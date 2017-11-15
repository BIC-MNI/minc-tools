#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "jpg_0xc3.h"

/* JPEG Lossless decoder, adapted from here:
 * http://www.mccauslandcenter.sc.edu/crnl/tools/jpeg-formats
 * Author: Chris Rorden
 *
 * This code decompresses the format used in the transfer syntax
 * "1.2.840.10008.1.2.4.70", which is an unusual form of lossless JPEG.
 */

static uint8_t
readByte(uint8_t *lRawRA,
         int *lRawPos,
         int lRawSz) {
  uint8_t ret = 0x00;
  if (*lRawPos < lRawSz)
    ret = lRawRA[*lRawPos];
  (*lRawPos)++;
  return ret;
}

static uint16_t
readWord(uint8_t *lRawRA,
         int *lRawPos,
         int lRawSz) {
  return (readByte(lRawRA, lRawPos, lRawSz) << 8) + readByte(lRawRA, lRawPos, lRawSz);
}

// Read the next single bit
static int
readBit(uint8_t *lRawRA,
        int *lRawPos,
        int *lCurrentBitPos) {
  int result = (lRawRA[*lRawPos] >> (7 - *lCurrentBitPos)) & 1;
  (*lCurrentBitPos)++;
  if (*lCurrentBitPos == 8) {
    (*lRawPos)++;
    *lCurrentBitPos = 0;
  }
  return result;
}

static const int bitMask[16] = {
  0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767
};


static int
readBits(uint8_t *lRawRA,
         int *lRawPos,
         int *lCurrentBitPos,
         int lNum              // lNum: bits to read, not to exceed 16
  ) {
  int result = lRawRA[*lRawPos];
  result = (result << 8) + lRawRA[(*lRawPos)+1];
  result = (result << 8) + lRawRA[(*lRawPos)+2];
  result = (result >> (24 - *lCurrentBitPos -lNum)) & bitMask[lNum]; //lCurrentBitPos is incremented from 1, so -1
  *lCurrentBitPos = *lCurrentBitPos + lNum;
  if (*lCurrentBitPos > 7) {
    *lRawPos = *lRawPos + (*lCurrentBitPos >> 3); // div 8
    *lCurrentBitPos = *lCurrentBitPos & 7; //mod 8
  }
  return result;
}

struct HufTables {
  uint8_t SSSSszRA[18];
  uint8_t LookUpRA[256];
  int DHTliRA[32];
  int DHTstartRA[32];
  int HufSz[32];
  int HufCode[32];
  int HufVal[32];
  int MaxHufSi;
  int MaxHufVal;
};

static int
decodePixelDifference(uint8_t *lRawRA,
                      int *lRawPos,
                      int *lCurrentBitPos,
                      struct HufTables ht) {
  int lByte = (lRawRA[*lRawPos] << *lCurrentBitPos) + (lRawRA[*lRawPos+1] >> (8- *lCurrentBitPos));
  lByte = lByte & 255;
  int lHufValSSSS = ht.LookUpRA[lByte];
  if (lHufValSSSS < 255) {
    *lCurrentBitPos = ht.SSSSszRA[lHufValSSSS] + *lCurrentBitPos;
    *lRawPos = *lRawPos + (*lCurrentBitPos >> 3);
    *lCurrentBitPos = *lCurrentBitPos & 7;
  } else { //full SSSS is not in the first 8-bits
    int lInput = lByte;
    int lInputBits = 8;
    (*lRawPos)++; // forward 8 bits = precisely 1 byte
    do {
      lInputBits++;
      lInput = (lInput << 1) + readBit(lRawRA, lRawPos, lCurrentBitPos);
      if (ht.DHTliRA[lInputBits] != 0) { //if any entries with this length
        int lI;
        for (lI = ht.DHTstartRA[lInputBits]; lI <= (ht.DHTstartRA[lInputBits]+ht.DHTliRA[lInputBits]-1); lI++) {
          if (lInput == ht.HufCode[lI])
            lHufValSSSS = ht.HufVal[lI];
        } //check each code
      } //if any entries with this length
      if ((lInputBits >= ht.MaxHufSi) && (lHufValSSSS > 254)) //exhausted options CR: added rev13
        lHufValSSSS = ht.MaxHufVal;
    } while (!(lHufValSSSS < 255)); // found;
  } //answer in first 8 bits
    //The HufVal is referred to as the SSSS in the Codec, so it is called 'lHufValSSSS'
  if (lHufValSSSS == 0) //NO CHANGE
    return 0;
  else if (lHufValSSSS == 16) { //ALL CHANGE 16 bit difference: Codec H.1.2.2 "No extra bits are appended after SSSS = 16 is encoded." Osiris Cornell Libraries fail here
    return 32768; //see H.1.2.2 and table H.2 of ISO 10918-1
  }
  //to get here - there is a 1..15 bit difference
  int lDiff = readBits(lRawRA, lRawPos, lCurrentBitPos, lHufValSSSS);
  if (lDiff <= bitMask[lHufValSSSS-1] )//add
    lDiff = lDiff - bitMask[lHufValSSSS];
  return lDiff;
}

uint8_t *
decode_jpeg_sof_0xc3(uint8_t *lRawRA, int lRawSz, int verbose, int *dimX, int *dimY, int *bits, int *frames) {
  uint8_t *lImgRA8 = NULL;
  if ((lRawRA[0] != 0xFF) || (lRawRA[1] != 0xD8) || (lRawRA[2] != 0xFF)) {
    printf("Error: JPEG signature 0xFFD8FF not found\n");
    return NULL;
  }
  //next: read header
  int lRawPos = 2; //Skip initial 0xFFD8, begin with third byte
  uint8_t btS1, btS2, SOSss, SOSse, SOSahal, SOSpttrans, btMarkerType, SOSns = 0x00; //tag
  uint8_t SOFnf, SOFprecision;
  uint16_t SOFydim, SOFxdim; //, lRestartSegmentSz;
  int lnHufTables;
  const int kmaxFrames = 4;
  struct HufTables ht[kmaxFrames+1];
  do { //read each marker in the header
    do {
      btS1 = readByte(lRawRA, &lRawPos, lRawSz);
      if (btS1 != 0xFF) {
        printf("JPEG header tag must begin with 0xFF\n");
        return NULL;
      }
      btMarkerType =  readByte(lRawRA, &lRawPos, lRawSz);
      if ((btMarkerType == 0x01) ||
          (btMarkerType == 0xFF) ||
          ((btMarkerType >= 0xD0) && (btMarkerType <= 0xD7) ) )
        btMarkerType = 0;   //only process segments with length fields
    } while ((lRawPos < lRawSz) && (btMarkerType == 0));
    uint16_t lSegmentLength = readWord(lRawRA, &lRawPos, lRawSz); //read marker length
    int lSegmentEnd = lRawPos+(lSegmentLength - 2);
    if (lSegmentEnd > lRawSz)  {
      return NULL;
    }
    if (verbose)
      printf("btMarkerType %#02X length %d@%d\n",
             btMarkerType, lSegmentLength, lRawPos);
    if ((btMarkerType >= 0xC0 && btMarkerType <= 0xC3) ||
        (btMarkerType >= 0xC5 && btMarkerType <= 0xCB) ||
        (btMarkerType >= 0xCD && btMarkerType <= 0xCF))  {
      //if Start-Of-Frame (SOF) marker
      SOFprecision = readByte(lRawRA, &lRawPos, lRawSz);
      SOFydim = readWord(lRawRA, &lRawPos, lRawSz);
      SOFxdim = readWord(lRawRA, &lRawPos, lRawSz);
      SOFnf = readByte(lRawRA, &lRawPos, lRawSz);
      lRawPos = (lSegmentEnd);
      if (verbose)
        printf(" [Precision %d X*Y %d*%d Frames %d]\n",
               SOFprecision, SOFxdim, SOFydim, SOFnf);
      if (btMarkerType != 0xC3) { //lImgTypeC3 = true;
        printf("This JPEG decoder can only decompress lossless JPEG ITU-T81 images (SoF must be 0XC3, not %#02X)\n", btMarkerType);
        return NULL;
      }
      if ((SOFprecision < 1) || (SOFprecision > 16) ||
          (SOFnf < 1) || (SOFnf == 2) || (SOFnf > 3)
          || ((SOFnf == 3) && (SOFprecision > 8))   ) {
        printf("Scalar data must be 1..16 bit, RGB data must be 8-bit (%d-bit, %d frames)\n", SOFprecision, SOFnf);
        return NULL;
      }
    } else if (btMarkerType == 0xC4) {// if SOF marker else if define-Huffman-tables marker (DHT)
      if (verbose)
        printf(" [Huffman Length %d]\n", lSegmentLength);
      int lFrameCount = 1;
      do {
        uint8_t DHTnLi = readByte(lRawRA, &lRawPos, lRawSz ); //we read but ignore DHTtcth.
        DHTnLi = 0;
        int lInc;
        for (lInc = 1; lInc <= 16; lInc++) {
          ht[lFrameCount].DHTliRA[lInc] = readByte(lRawRA, &lRawPos, lRawSz);
          DHTnLi = DHTnLi +  ht[lFrameCount].DHTliRA[lInc];
          if (ht[lFrameCount].DHTliRA[lInc] != 0)
            ht[lFrameCount].MaxHufSi = lInc;
        }
        if (DHTnLi > 17) {
          printf("Huffman table corrupted.\n");
          return NULL;
        }
        int lIncY = 0; //frequency
        for (lInc = 0; lInc <= 31; lInc++) {//lInc := 0 to 31 do begin
          ht[lFrameCount].HufVal[lInc] = -1;
          ht[lFrameCount].HufSz[lInc] = -1;
          ht[lFrameCount].HufCode[lInc] = -1;
        }
        for (lInc = 1; lInc <= 16; lInc++) {//set the huffman size values
          if (ht[lFrameCount].DHTliRA[lInc] > 0) {
            ht[lFrameCount].DHTstartRA[lInc] = lIncY+1;
            int lIncX;
            for (lIncX = 1; lIncX <= ht[lFrameCount].DHTliRA[lInc]; lIncX++) {
              lIncY++;
              btS1 = readByte(lRawRA, &lRawPos, lRawSz);
              ht[lFrameCount].HufVal[lIncY] = btS1;
              ht[lFrameCount].MaxHufVal = btS1;
              if (btS1 <= 16)
                ht[lFrameCount].HufSz[lIncY] = lInc;
              else {
                printf("Huffman size array corrupted.\n");
                return NULL;
              }
            }
          }
        } //set huffman size values
        int K = 1;
        int Code = 0;
        int Si = ht[lFrameCount].HufSz[K];
        do {
          while (Si == ht[lFrameCount].HufSz[K]) {
            ht[lFrameCount].HufCode[K] = Code;
            Code = Code + 1;
            K++;
          }
          if (K <= DHTnLi) {
            while (ht[lFrameCount].HufSz[K] > Si) {
              Code = Code << 1; //Shl!!!
              Si = Si + 1;
            } //while Si
          } //K <= 17
                    
        } while (K <= DHTnLi);
        lFrameCount++;
      } while ((lSegmentEnd-lRawPos) >= 18);
      lnHufTables = lFrameCount - 1;
      lRawPos = (lSegmentEnd);
      if (verbose) printf(" [FrameCount %d]\n", lnHufTables);
    } else if (btMarkerType == 0xDD) {  //if DHT marker else if Define restart interval (DRI) marker
      printf("This image uses Restart Segments - not supported.\n");
      return NULL;
    } else if (btMarkerType == 0xDA) {  //if DRI marker else if read Start of Scan (SOS) marker
      SOSns = readByte(lRawRA, &lRawPos, lRawSz);
      //if Ns = 1 then NOT interleaved, else interleaved: see B.2.3
      if (SOSns > 0) {
        int lInc;
        for (lInc = 1; lInc <= SOSns; lInc++) {
          btS1 = readByte(lRawRA, &lRawPos, lRawSz); //component identifier 1=Y,2=Cb,3=Cr,4=I,5=Q
          btS2 = readByte(lRawRA, &lRawPos, lRawSz); //horizontal and vertical sampling factors
        }
      }
      SOSss = readByte(lRawRA, &lRawPos, lRawSz); //predictor selection B.3
      SOSse = readByte(lRawRA, &lRawPos, lRawSz); /* ignored */
      SOSahal = readByte(lRawRA, &lRawPos, lRawSz); //lower 4bits= pointtransform
      SOSpttrans = SOSahal & 16;
      if (verbose)
        printf(" [Predictor: %d Unknown: %d Transform %d]\n", SOSss, SOSse, SOSahal);
      lRawPos = (lSegmentEnd);
    } else  //if SOS marker else skip marker
      lRawPos = (lSegmentEnd);
  } while ((lRawPos < lRawSz) && (btMarkerType != 0xDA)); //0xDA=Start of scan: loop for reading header
    //NEXT: Huffman decoding
  if (lnHufTables < 1) {
    printf("Decoding error: no Huffman tables.\n");
    return NULL;
  }
  //NEXT: unpad data - delete byte that follows $FF
  int lIncI = lRawPos; //input position
  int lIncO = lRawPos; //output position
  do {
    lRawRA[lIncO] = lRawRA[lIncI];
    if (lRawRA[lIncI] == 255) {
      if (lRawRA[lIncI+1] == 0)
        lIncI = lIncI+1;
      else if (lRawRA[lIncI+1] == 0xD9)
        lIncO = -666; //end of padding
    }
    lIncI++;
    lIncO++;
  } while (lIncO > 0);
  //NEXT: some RGB images use only a single Huffman table for all 3 colour planes. In this case, replicate the correct values
  //NEXT: prepare lookup table
  int lFrameCount;
  for (lFrameCount = 1; lFrameCount <= lnHufTables; lFrameCount++) {
    int lInc;
    for (lInc = 0; lInc <= 17; lInc++)
      ht[lFrameCount].SSSSszRA[lInc] = 123; //Impossible value for SSSS, suggests 8-bits can not describe answer
    for (lInc = 0; lInc <= 255; lInc++)
      ht[lFrameCount].LookUpRA[lInc] = 255; //Impossible value for SSSS, suggests 8-bits can not describe answer
  }
  //NEXT: fill lookuptable
    
  for (lFrameCount = 1; lFrameCount <= lnHufTables; lFrameCount++) {
    int lIncY = 0;
    int lSz;
    for (lSz = 1; lSz <= 8; lSz++) { //set the huffman lookup table for keys with lengths <=8
      if (ht[lFrameCount].DHTliRA[lSz]> 0) {
        int lIncX;
        for (lIncX = 1; lIncX <= ht[lFrameCount].DHTliRA[lSz]; lIncX++) {
          lIncY++;
          int lHufVal = ht[lFrameCount].HufVal[lIncY]; //SSSS
          ht[lFrameCount].SSSSszRA[lHufVal] = lSz;
          int k = (ht[lFrameCount].HufCode[lIncY] << (8-lSz )) & 255; //K= most sig bits for hufman table
          if (lSz < 8) { //fill in all possible bits that exceed the huffman table
            int lInc = bitMask[8-lSz];
            int lCurrentBitPos;
            for (lCurrentBitPos = 0; lCurrentBitPos <= lInc; lCurrentBitPos++) {
              ht[lFrameCount].LookUpRA[k+lCurrentBitPos] = lHufVal;
            }
          } else
            ht[lFrameCount].LookUpRA[k] = lHufVal; //SSSS
        } //Set SSSS
      } //Length of size lInc > 0
    } //for lInc := 1 to 8
  } //For each frame, e.g. once each for Red/Green/Blue
    //NEXT: some RGB images use only a single Huffman table for all 3 colour planes. In this case, replicate the correct values
  if (lnHufTables < SOFnf) { //use single Hufman table for each frame
    int lFrameCount;
    for (lFrameCount = 2; lFrameCount <= SOFnf; lFrameCount++) {
      ht[lFrameCount] = ht[1];
    } //for each frame
  } // if lnHufTables < SOFnf
    //NEXT: uncompress data: different loops for different predictors
  int lItems =  SOFxdim*SOFydim*SOFnf;
  // lRawPos++;// <- only for Pascal where array is indexed from 1 not 0 first byte of data
  int lCurrentBitPos = 0; //read in a new byte
    
  //depending on SOSss, we see Table H.1
  int lPredA = 0;
  int lPredB = 0;
  int lPredC = 0;
  if (SOSss == 2) //predictor selection 2: above
    lPredA = SOFxdim-1;
  else if (SOSss == 3) //predictor selection 3: above+left
    lPredA = SOFxdim;
  else if ((SOSss == 4) || (SOSss == 5)) { //these use left, above and above+left WEIGHT LEFT
    lPredA = 0; //Ra left
    lPredB = SOFxdim-1; //Rb directly above
    lPredC = SOFxdim; //Rc UpperLeft:above and to the left
  } else if (SOSss == 6) { //also use left, above and above+left, WEIGHT ABOVE
    lPredB = 0;
    lPredA = SOFxdim-1; //Rb directly above
    lPredC = SOFxdim; //Rc UpperLeft:above and to the left
  }   else
    lPredA = 0; //Ra: directly to left)
  if (SOFprecision > 8) { //start - 16 bit data
    *bits = 16;
    int lPx = -1; //pixel position
    int lPredicted =  1 << (SOFprecision-1-SOSpttrans);
    lImgRA8 = (uint8_t*) malloc(lItems * 2);
    uint16_t *lImgRA16 = (uint16_t*) lImgRA8;
    int i;
    for (i = 0; i < lItems; i++)
      lImgRA16[i] = 0; //zero array
    int frame = 1;
    int lIncX;
    for (lIncX = 1; lIncX <= SOFxdim; lIncX++) { //for first row - here we ALWAYS use LEFT as predictor
      lPx++; //writenext voxel
      if (lIncX > 1)
        lPredicted = lImgRA16[lPx-1];
      lImgRA16[lPx] = lPredicted + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[frame]);
    }
    int lIncY;
    for (lIncY = 2; lIncY <= SOFydim; lIncY++) {//for all subsequent rows
      lPx++; //write next voxel
      lPredicted = lImgRA16[lPx-SOFxdim]; //use ABOVE
      lImgRA16[lPx] = lPredicted + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[frame]);
      if (SOSss == 4) {
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPredicted = lImgRA16[lPx - lPredA] + lImgRA16[lPx - lPredB] - lImgRA16[lPx - lPredC];
          lPx++; //writenext voxel
          lImgRA16[lPx] = lPredicted + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[frame]);
        } //for lIncX
      } else if ((SOSss == 5) || (SOSss == 6)) {
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPredicted = lImgRA16[lPx - lPredA] + ((lImgRA16[lPx - lPredB] - lImgRA16[lPx - lPredC]) >> 1);
          lPx++; //writenext voxel
          lImgRA16[lPx] = lPredicted + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[frame]);
        } //for lIncX
      } else if (SOSss == 7) {
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPx++; //writenext voxel
          lPredicted = (lImgRA16[lPx-1]+lImgRA16[lPx-SOFxdim]) >> 1;
          lImgRA16[lPx] = lPredicted+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[frame]);
        } //for lIncX
      } else { //SOSss 1,2,3 read single values
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPredicted = lImgRA16[lPx-lPredA];
          lPx++; //writenext voxel
          lImgRA16[lPx] = lPredicted+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[frame]);
        } //for lIncX
      } // if..else possible predictors
    }//for lIncY
  } else if (SOFnf == 3) { //if 16-bit data; else 8-bit 3 frames
    *bits = 8;
    lImgRA8 = (uint8_t*) malloc(lItems );
    int lPx[kmaxFrames+1], lPredicted[kmaxFrames+1]; //pixel position
    int f;
    for (f = 1; f <= SOFnf; f++) {
      lPx[f] = ((f-1) * (SOFxdim * SOFydim) ) -1;
      lPredicted[f] = 1 << (SOFprecision-1-SOSpttrans);
    }
    int i;
    for (i = 0; i < lItems; i++)
      lImgRA8[i] = 255; //zero array
    int lIncX;
    for (lIncX = 1; lIncX <= SOFxdim; lIncX++) { //for first row - here we ALWAYS use LEFT as predictor
      int f;
      for (f = 1; f <= SOFnf; f++) {
        lPx[f]++; //writenext voxel
        if (lIncX > 1) lPredicted[f] = lImgRA8[lPx[f]-1];
        lImgRA8[lPx[f]] = lPredicted[f] + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[f]);
      }
    } //first row always predicted by LEFT
    int lIncY;
    for (lIncY = 2; lIncY <= SOFydim; lIncY++) {//for all subsequent rows
      int f;
      for (f = 1; f <= SOFnf; f++) {
        lPx[f]++; //write next voxel
        lPredicted[f] = lImgRA8[lPx[f]-SOFxdim]; //use ABOVE
        lImgRA8[lPx[f]] = lPredicted[f] + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[f]);
      }//first column of row always predicted by ABOVE
      if (SOSss == 4) {
        int lIncX;
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          int f;
          for (f = 1; f <= SOFnf; f++) {
            lPredicted[f] = lImgRA8[lPx[f]-lPredA]+lImgRA8[lPx[f]-lPredB]-lImgRA8[lPx[f]-lPredC];
            lPx[f]++; //writenext voxel
            lImgRA8[lPx[f]] = lPredicted[f]+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[f]);
          }
        } //for lIncX
      } else if ((SOSss == 5) || (SOSss == 6)) {
        int lIncX;
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          int f;
          for (f = 1; f <= SOFnf; f++) {
            lPredicted[f] = lImgRA8[lPx[f]-lPredA]+ ((lImgRA8[lPx[f]-lPredB]-lImgRA8[lPx[f]-lPredC]) >> 1);
            lPx[f]++; //writenext voxel
            lImgRA8[lPx[f]] = lPredicted[f] + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[f]);
          }
        } //for lIncX
      } else if (SOSss == 7) {
        int lIncX;
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          int f;
          for (f = 1; f <= SOFnf; f++) {
            lPx[f]++; //writenext voxel
            lPredicted[f] = (lImgRA8[lPx[f]-1]+lImgRA8[lPx[f]-SOFxdim]) >> 1;
            lImgRA8[lPx[f]] = lPredicted[f] + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[f]);
          }
        } //for lIncX
      } else { //SOSss 1,2,3 read single values
        int lIncX;
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          int f;
          for (f = 1; f <= SOFnf; f++) {
            lPredicted[f] = lImgRA8[lPx[f]-lPredA];
            lPx[f]++; //writenext voxel
            lImgRA8[lPx[f]] = lPredicted[f] + decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[f]);
          }
        } //for lIncX
      } // if..else possible predictors
    }//for lIncY
  }else { //if 8-bit data 3frames; else 8-bit 1 frames
    *bits = 8;
    lImgRA8 = (uint8_t*) malloc(lItems);
    int lPx = -1; //pixel position
    int lPredicted =  1 << (SOFprecision-1-SOSpttrans);
    int i;
    for (i = 0; i < lItems; i++)
      lImgRA8[i] = 0; //zero array
    int lIncX;
    for (lIncX = 1; lIncX <= SOFxdim; lIncX++) { //for first row - here we ALWAYS use LEFT as predictor
      lPx++; //writenext voxel
      if (lIncX > 1) lPredicted = lImgRA8[lPx-1];
      int dx = decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[1]);
      lImgRA8[lPx] = lPredicted+dx;
    }
    int lIncY;
    for (lIncY = 2; lIncY <= SOFydim; lIncY++) {//for all subsequent rows
      lPx++; //write next voxel
      lPredicted = lImgRA8[lPx-SOFxdim]; //use ABOVE
      lImgRA8[lPx] = lPredicted+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[1]);
      if (SOSss == 4) {
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPredicted = lImgRA8[lPx-lPredA]+lImgRA8[lPx-lPredB]-lImgRA8[lPx-lPredC];
          lPx++; //writenext voxel
          lImgRA8[lPx] = lPredicted+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[1]);
        } //for lIncX
      } else if ((SOSss == 5) || (SOSss == 6)) {
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPredicted = lImgRA8[lPx-lPredA]+ ((lImgRA8[lPx-lPredB]-lImgRA8[lPx-lPredC]) >> 1);
          lPx++; //writenext voxel
          lImgRA8[lPx] = lPredicted+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[1]);
        } //for lIncX
      } else if (SOSss == 7) {
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPx++; //writenext voxel
          lPredicted = (lImgRA8[lPx-1]+lImgRA8[lPx-SOFxdim]) >> 1;
          lImgRA8[lPx] = lPredicted+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[1]);
        } //for lIncX
      } else { //SOSss 1,2,3 read single values
        for (lIncX = 2; lIncX <= SOFxdim; lIncX++) {
          lPredicted = lImgRA8[lPx-lPredA];
          lPx++; //writenext voxel
          lImgRA8[lPx] = lPredicted+decodePixelDifference(lRawRA, &lRawPos, &lCurrentBitPos, ht[1]);
        } //for lIncX
      } // if..else possible predictors
    }//for lIncY
  } //if 16bit else 8bit
  *dimX = SOFxdim;
  *dimY = SOFydim;
  *frames = SOFnf;
  return lImgRA8;
} //end decode_JPEG_SOF_0XC3()

