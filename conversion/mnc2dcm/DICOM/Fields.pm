# DICOM::Fields - Definitions of DICOM fields.
#
# Andrew Crabb (ahc@jhu.edu), May 2002
# Andrew Janke (a.janke@gmail.com), March 2009
# $Id: Fields.pm,v 1.6 2009/03/17 23:49:09 rotor Exp $

package DICOM::Fields;

use strict;
use warnings;
use Pod::Usage;

require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(@dicom_fields);

our $VERSION = sprintf "%d.%03d", q$Revision: 1.6 $ =~ /: (\d+)\.(\d+)/;

# ------------------------------------------------------------------------
# NOT FOR MEDICAL USE.  This file is provided purely for experimental use.
# ------------------------------------------------------------------------
# key
#   group     group (hex)
#   element   element within group (hex)
#   VR        Value Representation
#   multiple  number of items allowed
#   name      field name

our @dicom_fields = <<END_DICOM_FIELDS =~ m/(\S.*\S)/g;
0000   0000   UL   1      GroupLength
0000   0001   UL   1      CommandLengthToEnd
0000   0002   UI   1      AffectedSOPClassUID
0000   0003   UI   1      RequestedSOPClassUID
0000   0010   CS   1      CommandRecognitionCode
0000   0100   US   1      CommandField 
0000   0110   US   1      MessageID
0000   0120   US   1      MessageIDBeingRespondedTo
0000   0200   AE   1      Initiator
0000   0300   AE   1      Receiver
0000   0400   AE   1      FindLocation
0000   0600   AE   1      MoveDestination
0000   0700   US   1      Priority
0000   0800   US   1      DataSetType
0000   0850   US   1      NumberOfMatches
0000   0860   US   1      ResponseSequenceNumber
0000   0900   US   1      Status
0000   0901   AT   1-n    OffendingElement
0000   0902   LO   1      ErrorComment
0000   0903   US   1      ErrorID
0000   0904   OT   1-n    ErrorInformation
0000   1000   UI   1      AffectedSOPInstanceUID
0000   1001   UI   1      RequestedSOPInstanceUID
0000   1002   US   1      EventTypeID
0000   1003   OT   1-n    EventInformation
0000   1005   AT   1-n    AttributeIdentifierList
0000   1007   AT   1-n    ModificationList
0000   1008   US   1      ActionTypeID
0000   1009   OT   1-n    ActionInformation
0000   1013   UI   1-n    SuccessfulSOPInstanceUIDList
0000   1014   UI   1-n    FailedSOPInstanceUIDList
0000   1015   UI   1-n    WarningSOPInstanceUIDList
0000   1020   US   1      NumberOfRemainingSuboperations
0000   1021   US   1      NumberOfCompletedSuboperations
0000   1022   US   1      NumberOfFailedSuboperations
0000   1023   US   1      NumberOfWarningSuboperations
0000   1030   AE   1      MoveOriginatorApplicationEntityTitle
0000   1031   US   1      MoveOriginatorMessageID
0000   4000   AT   1      DialogReceiver
0000   4010   AT   1      TerminalType
0000   5010   SH   1      MessageSetID
0000   5020   SH   1      EndMessageSet
0000   5110   AT   1      DisplayFormat
0000   5120   AT   1      PagePositionID
0000   5130   CS   1      TextFormatID
0000   5140   CS   1      NormalReverse
0000   5150   CS   1      AddGrayScale
0000   5160   CS   1      Borders
0000   5170   IS   1      Copies
0000   5180   CS   1      OldMagnificationType
0000   5190   CS   1      Erase
0000   51A0   CS   1      Print
0000   51B0   US   1-n    Overlays
0002   0000   UL   1      MetaElementGroupLength
0002   0001   OB   1      FileMetaInformationVersion
0002   0002   UI   1      MediaStorageSOPClassUID
0002   0003   UI   1      MediaStorageSOPInstanceUID
0002   0010   UI   1      TransferSyntaxUID
0002   0012   UI   1      ImplementationClassUID
0002   0013   SH   1      ImplementationVersionName
0002   0016   AE   1      SourceApplicationEntityTitle
0002   0100   UI   1      PrivateInformationCreatorUID
0002   0102   OB   1      PrivateInformation
0004   0000   UL   1      FileSetGroupLength
0004   1130   CS   1      FileSetID
0004   1141   CS   8      FileSetDescriptorFileID
0004   1142   CS   1      FileSetCharacterSet
0004   1200   UL   1      RootDirectoryFirstRecord
0004   1202   UL   1      RootDirectoryLastRecord
0004   1212   US   1      FileSetConsistencyFlag
0004   1220   SQ   1      DirectoryRecordSequence
0004   1400   UL   1      NextDirectoryRecordOffset
0004   1410   US   1      RecordInUseFlag
0004   1420   UL   1      LowerLevelDirectoryOffset
0004   1430   CS   1      DirectoryRecordType
0004   1432   UI   1      PrivateRecordUID
0004   1500   CS   8      ReferencedFileID
0004   1504   UL   1      DirectoryRecordOffset
0004   1510   UI   1      ReferencedSOPClassUIDInFile
0004   1511   UI   1      ReferencedSOPInstanceUIDInFile
0004   1512   UI   1      ReferencedTransferSyntaxUIDInFile
0004   1600   UL   1      NumberOfReferences
0008   0000   UL   1      IdentifyingGroupLength
0008   0001   UL   1      LengthToEnd
0008   0005   CS   1      SpecificCharacterSet
0008   0008   CS   1-n    ImageType
0008   000A   US   1      SequenceItemNumber
0008   0010   CS   1      RecognitionCode
0008   0012   DA   1      InstanceCreationDate
0008   0013   TM   1      InstanceCreationTime
0008   0014   UI   1      InstanceCreatorUID
0008   0016   UI   1      SOPClassUID
0008   0018   UI   1      SOPInstanceUID
0008   0020   DA   1      StudyDate
0008   0021   DA   1      SeriesDate
0008   0022   DA   1      AcquisitionDate
0008   0023   DA   1      ImageDate
0008   0024   DA   1      OverlayDate
0008   0025   DA   1      CurveDate
0008   002A   DT   1      AcquisitionDatetime
0008   0030   TM   1      StudyTime
0008   0031   TM   1      SeriesTime
0008   0032   TM   1      AcquisitionTime
0008   0033   TM   1      ContentTime
0008   0034   TM   1      OverlayTime
0008   0035   TM   1      CurveTime
0008   0040   US   1      OldDataSetType
0008   0041   LT   1      OldDataSetSubtype
0008   0042   CS   1      NuclearMedicineSeriesType
0008   0050   SH   1      AccessionNumber
0008   0052   CS   1      QueryRetrieveLevel
0008   0054   AE   1-n    RetrieveAETitle
0008   0058   UI   1-n    DataSetFailedSOPInstanceUIDList
0008   0060   CS   1      Modality
0008   0061   CS   1-n    ModalitiesInStudy
0008   0064   CS   1      ConversionType
0008   0068   CS   1      PresentationIntentType
0008   0070   LO   1      Manufacturer
0008   0080   LO   1      InstitutionName
0008   0081   ST   1      InstitutionAddress
0008   0082   SQ   1      InstitutionCodeSequence
0008   0090   PN   1      ReferringPhysicianName
0008   0092   ST   1      ReferringPhysicianAddress
0008   0094   SH   1-n    ReferringPhysicianTelephoneNumber
0008   0100   SH   1      CodeValue
0008   0102   SH   1      CodingSchemeDesignator
0008   0103   SH   1      CodingSchemeVersion
0008   0104   LO   1      CodeMeaning
0008   0105   CS   1      MappingResource
0008   0106   DT   1      ContextGroupVersion
0008   0107   DT   1      ContextGroupLocalVersion
0008   010B   CS   1      CodeSetExtensionFlag
0008   010C   UI   1      PrivateCodingSchemeCreatorUID
0008   010D   UI   1      CodeSetExtensionCreatorUID
0008   010F   CS   1      ContextIdentifier
0008   0201   SH   1      TimezoneOffsetFromUTC
0008   1000   AE   1      NetworkID
0008   1010   SH   1      StationName
0008   1030   LO   1      StudyDescription
0008   1032   SQ   1      ProcedureCodeSequence
0008   103E   LO   1      SeriesDescription
0008   1040   LO   1      InstitutionalDepartmentName
0008   1048   PN   1-n    PhysicianOfRecord
0008   1050   PN   1-n    PerformingPhysicianName
0008   1060   PN   1-n    PhysicianReadingStudy
0008   1070   PN   1-n    OperatorName
0008   1080   LO   1-n    AdmittingDiagnosisDescription
0008   1084   SQ   1      AdmittingDiagnosisCodeSequence
0008   1090   LO   1      ManufacturerModelName
0008   1100   SQ   1      ReferencedResultsSequence
0008   1110   SQ   1      ReferencedStudySequence
0008   1111   SQ   1      ReferencedStudyComponentSequence
0008   1115   SQ   1      ReferencedSeriesSequence
0008   1120   SQ   1      ReferencedPatientSequence
0008   1125   SQ   1      ReferencedVisitSequence
0008   1130   SQ   1      ReferencedOverlaySequence
0008   1140   SQ   1      ReferencedImageSequence
0008   1145   SQ   1      ReferencedCurveSequence
0008   114A   SQ   1      ReferencedInstanceSequence
0008   114B   LO   1      ReferenceDescription
0008   1150   UI   1      ReferencedSOPClassUID
0008   1155   UI   1      ReferencedSOPInstanceUID
0008   115A   UI   1-n    SOPClassesSupported
0008   1160   IS   1      ReferencedFrameNumber
0008   1195   UI   1      TransactionUID
0008   1197   US   1      FailureReason
0008   1198   SQ   1      FailedSOPSequence
0008   1199   SQ   1      ReferencedSOPSequence
0008   2110   CS   1      LossyImageCompression
0008   2111   ST   1      DerivationDescription
0008   2112   SQ   1      SourceImageSequence
0008   2120   SH   1      StageName
0008   2122   IS   1      StageNumber
0008   2124   IS   1      NumberOfStages
0008   2128   IS   1      ViewNumber
0008   2129   IS   1      NumberOfEventTimers
0008   212A   IS   1      NumberOfViewsInStage
0008   2130   DS   1-n    EventElapsedTime
0008   2132   LO   1-n    EventTimerName
0008   2142   IS   1      StartTrim
0008   2143   IS   1      StopTrim
0008   2144   IS   1      RecommendedDisplayFrameRate
0008   2200   CS   1      TransducerPosition
0008   2204   CS   1      TransducerOrientation
0008   2208   CS   1      AnatomicStructure
0008   2218   SQ   1      AnatomicRegionSequence
0008   2220   SQ   1      AnatomicRegionModifierSequence
0008   2228   SQ   1      PrimaryAnatomicStructureSequence
0008   2229   SQ   1      AnatomicStructureSpaceOrRegionSequence
0008   2230   SQ   1      PrimaryAnatomicStructureModifierSequence
0008   2240   SQ   1      TransducerPositionSequence
0008   2242   SQ   1      TransducerPositionModifierSequence
0008   2244   SQ   1      TransducerOrientationSequence
0008   2246   SQ   1      TransducerOrientationModifierSequence
0008   4000   LT   1-n    IdentifyingComments
0010   0000   UL   1      PatientGroupLength
0010   0010   PN   1      PatientName
0010   0020   LO   1      PatientID
0010   0021   LO   1      IssuerOfPatientID
0010   0030   DA   1      PatientBirthDate
0010   0032   TM   1      PatientBirthTime
0010   0040   CS   1      PatientSex
0010   0050   SQ   1      PatientInsurancePlanCodeSequence
0010   1000   LO   1-n    OtherPatientID
0010   1001   PN   1-n    OtherPatientName
0010   1005   PN   1      PatientBirthName
0010   1010   AS   1      PatientAge
0010   1020   DS   1      PatientSize
0010   1030   DS   1      PatientWeight
0010   1040   LO   1      PatientAddress
0010   1050   LT   1-n    InsurancePlanIdentification
0010   1060   PN   1      PatientMotherBirthName
0010   1080   LO   1      MilitaryRank
0010   1081   LO   1      BranchOfService
0010   1090   LO   1      MedicalRecordLocator
0010   2000   LO   1-n    MedicalAlerts
0010   2110   LO   1-n    ContrastAllergies
0010   2150   LO   1      CountryOfResidence
0010   2152   LO   1      RegionOfResidence
0010   2154   SH   1-n    PatientTelephoneNumber
0010   2160   SH   1      EthnicGroup
0010   2180   SH   1      Occupation
0010   21A0   CS   1      SmokingStatus
0010   21B0   LT   1      AdditionalPatientHistory
0010   21C0   US   1      PregnancyStatus
0010   21D0   DA   1      LastMenstrualDate
0010   21F0   LO   1      PatientReligiousPreference
0010   4000   LT   1      PatientComments
0018   0000   UL   1      AcquisitionGroupLength
0018   0010   LO   1      ContrastBolusAgent
0018   0012   SQ   1      ContrastBolusAgentSequence
0018   0014   SQ   1      ContrastBolusAdministrationRouteSequence
0018   0015   CS   1      BodyPartExamined
0018   0020   CS   1-n    ScanningSequence
0018   0021   CS   1-n    SequenceVariant
0018   0022   CS   1-n    ScanOptions
0018   0023   CS   1      MRAcquisitionType
0018   0024   SH   1      SequenceName
0018   0025   CS   1      AngioFlag
0018   0026   SQ   1      InterventionDrugInformationSequence
0018   0027   TM   1      InterventionDrugStopTime
0018   0028   DS   1      InterventionDrugDose
0018   0029   SQ   1      InterventionalDrugSequence
0018   002A   SQ   1      AdditionalDrugSequence
0018   0030   LO   1-n    Radionuclide
0018   0031   LO   1-n    Radiopharmaceutical
0018   0032   DS   1      EnergyWindowCenterline
0018   0033   DS   1-n    EnergyWindowTotalWidth
0018   0034   LO   1      InterventionalDrugName
0018   0035   TM   1      InterventionalDrugStartTime
0018   0036   SQ   1      InterventionalTherapySequence
0018   0037   CS   1      TherapyType
0018   0038   CS   1      InterventionalStatus
0018   0039   CS   1      TherapyDescription
0018   0040   IS   1      CineRate
0018   0050   DS   1      SliceThickness
0018   0060   DS   1      KVP
0018   0070   IS   1      CountsAccumulated
0018   0071   CS   1      AcquisitionTerminationCondition
0018   0072   DS   1      EffectiveSeriesDuration
0018   0073   CS   1      AcquisitionStartCondition
0018   0074   IS   1      AcquisitionStartConditionData
0018   0075   IS   1      AcquisitionTerminationConditionData
0018   0080   DS   1      RepetitionTime
0018   0081   DS   1      EchoTime
0018   0082   DS   1      InversionTime
0018   0083   DS   1      NumberOfAverages
0018   0084   DS   1      ImagingFrequency
0018   0085   SH   1      ImagedNucleus
0018   0086   IS   1-n    EchoNumber
0018   0087   DS   1      MagneticFieldStrength
0018   0088   DS   1      SpacingBetweenSlices
0018   0089   IS   1      NumberOfPhaseEncodingSteps
0018   0090   DS   1      DataCollectionDiameter
0018   0091   IS   1      EchoTrainLength
0018   0093   DS   1      PercentSampling
0018   0094   DS   1      PercentPhaseFieldOfView
0018   0095   DS   1      PixelBandwidth
0018   1000   LO   1      DeviceSerialNumber
0018   1004   LO   1      PlateID
0018   1010   LO   1      SecondaryCaptureDeviceID
0018   1011   LO   1      HardcopyCreationDeviceID
0018   1012   DA   1      DateOfSecondaryCapture
0018   1014   TM   1      TimeOfSecondaryCapture
0018   1016   LO   1      SecondaryCaptureDeviceManufacturer
0018   1017   LO   1      HardcopyDeviceManufacturer
0018   1018   LO   1      SecondaryCaptureDeviceManufacturerModelName
0018   1019   LO   1-n    SecondaryCaptureDeviceSoftwareVersion
0018   101A   LO   1-n    HardcopyDeviceSoftwareVersion
0018   101B   LO   1      HardcopyDeviceManfuacturersModelName
0018   1020   LO   1-n    SoftwareVersion
0018   1022   SH   1      VideoImageFormatAcquired
0018   1023   LO   1      DigitalImageFormatAcquired
0018   1030   LO   1      ProtocolName
0018   1040   LO   1      ContrastBolusRoute
0018   1041   DS   1      ContrastBolusVolume
0018   1042   TM   1      ContrastBolusStartTime
0018   1043   TM   1      ContrastBolusStopTime
0018   1044   DS   1      ContrastBolusTotalDose
0018   1045   IS   1-n    SyringeCounts
0018   1046   DS   1-n    ContrastFlowRate
0018   1047   DS   1-n    ContrastFlowDuration
0018   1048   CS   1      ContrastBolusIngredient
0018   1049   DS   1      ContrastBolusIngredientConcentration
0018   1050   DS   1      SpatialResolution
0018   1060   DS   1      TriggerTime
0018   1061   LO   1      TriggerSourceOrType
0018   1062   IS   1      NominalInterval
0018   1063   DS   1      FrameTime
0018   1064   LO   1      FramingType
0018   1065   DS   1-n    FrameTimeVector
0018   1066   DS   1      FrameDelay
0018   1067   DS   1      ImageTriggerDelay
0018   1068   DS   1      MultiplexGroupTimeOffset
0018   1069   DS   1      TriggerTimeOffset
0018   106A   CS   1      SynchronizationTrigger
0018   106C   US   2      SynchronizationChannel
0018   106E   UL   1      TriggerSamplePosition
0018   1070   LO   1-n    RadionuclideRoute
0018   1071   DS   1-n    RadionuclideVolume
0018   1072   TM   1-n    RadionuclideStartTime
0018   1073   TM   1-n    RadionuclideStopTime
0018   1074   DS   1-n    RadionuclideTotalDose
0018   1075   DS   1      RadionuclideHalfLife
0018   1076   DS   1      RadionuclidePositronFraction
0018   1077   DS   1      RadiopharmaceuticalSpecificActivity
0018   1080   CS   1      BeatRejectionFlag
0018   1081   IS   1      LowRRValue
0018   1082   IS   1      HighRRValue
0018   1083   IS   1      IntervalsAcquired
0018   1084   IS   1      IntervalsRejected
0018   1085   LO   1      PVCRejection
0018   1086   IS   1      SkipBeats
0018   1088   IS   1      HeartRate
0018   1090   IS   1      CardiacNumberOfImages
0018   1094   IS   1      TriggerWindow
0018   1100   DS   1      ReconstructionDiameter
0018   1110   DS   1      DistanceSourceToDetector
0018   1111   DS   1      DistanceSourceToPatient
0018   1114   DS   1      EstimatedRadiographicMagnificationFactor
0018   1120   DS   1      GantryDetectorTilt
0018   1121   DS   1      GantryDetectorSlew
0018   1130   DS   1      TableHeight
0018   1131   DS   1      TableTraverse
0018   1134   DS   1      TableMotion
0018   1135   DS   1-n    TableVerticalIncrement
0018   1136   DS   1-n    TableLateralIncrement
0018   1137   DS   1-n    TableLongitudinalIncrement
0018   1138   DS   1      TableAngle
0018   113A   CS   1      TableType
0018   1140   CS   1      RotationDirection
0018   1141   DS   1      AngularPosition
0018   1142   DS   1-n    RadialPosition
0018   1143   DS   1      ScanArc
0018   1144   DS   1      AngularStep
0018   1145   DS   1      CenterOfRotationOffset
0018   1146   DS   1-n    RotationOffset
0018   1147   CS   1      FieldOfViewShape
0018   1149   IS   2      FieldOfViewDimension
0018   1150   IS   1      ExposureTime
0018   1151   IS   1      XrayTubeCurrent
0018   1152   IS   1      Exposure
0018   1153   IS   1      ExposureinuAs
0018   1154   DS   1      AveragePulseWidth
0018   1155   CS   1      RadiationSetting
0018   1156   CS   1      RectificationType
0018   115A   CS   1      RadiationMode
0018   115E   DS   1      ImageAreaDoseProduct
0018   1160   SH   1      FilterType
0018   1161   LO   1-n    TypeOfFilters
0018   1162   DS   1      IntensifierSize
0018   1164   DS   2      ImagerPixelSpacing
0018   1166   CS   1      Grid
0018   1170   IS   1      GeneratorPower
0018   1180   SH   1      CollimatorGridName
0018   1181   CS   1      CollimatorType
0018   1182   IS   1      FocalDistance
0018   1183   DS   1      XFocusCenter
0018   1184   DS   1      YFocusCenter
0018   1190   DS   1-n    FocalSpot
0018   1191   CS   1      AnodeTargetMaterial
0018   11A0   DS   1      BodyPartThickness
0018   11A2   DS   1      CompressionForce
0018   1200   DA   1-n    DateOfLastCalibration
0018   1201   TM   1-n    TimeOfLastCalibration
0018   1210   SH   1-n    ConvolutionKernel
0018   1240   IS   1-n    UpperLowerPixelValues
0018   1242   IS   1      ActualFrameDuration
0018   1243   IS   1      CountRate
0018   1244   US   1      PreferredPlaybackSequencing
0018   1250   SH   1      ReceivingCoil
0018   1251   SH   1      TransmittingCoil
0018   1260   SH   1      PlateType
0018   1261   LO   1      PhosphorType
0018   1300   IS   1      ScanVelocity
0018   1301   CS   1-n    WholeBodyTechnique
0018   1302   IS   1      ScanLength
0018   1310   US   4      AcquisitionMatrix
0018   1312   CS   1      PhaseEncodingDirection
0018   1314   DS   1      FlipAngle
0018   1315   CS   1      VariableFlipAngleFlag
0018   1316   DS   1      SAR
0018   1318   DS   1      dBdt
0018   1400   LO   1      AcquisitionDeviceProcessingDescription
0018   1401   LO   1      AcquisitionDeviceProcessingCode
0018   1402   CS   1      CassetteOrientation
0018   1403   CS   1      CassetteSize
0018   1404   US   1      ExposuresOnPlate
0018   1405   IS   1      RelativeXrayExposure
0018   1450   DS   1      ColumnAngulation
0018   1460   DS   1      TomoLayerHeight
0018   1470   DS   1      TomoAngle
0018   1480   DS   1      TomoTime
0018   1490   CS   1      TomoType
0018   1491   CS   1      TomoClass
0018   1495   IS   1      NumberofTomosynthesisSourceImages
0018   1500   CS   1      PositionerMotion
0018   1508   CS   1      PositionerType
0018   1510   DS   1      PositionerPrimaryAngle
0018   1511   DS   1      PositionerSecondaryAngle
0018   1520   DS   1-n    PositionerPrimaryAngleIncrement
0018   1521   DS   1-n    PositionerSecondaryAngleIncrement
0018   1530   DS   1      DetectorPrimaryAngle
0018   1531   DS   1      DetectorSecondaryAngle
0018   1600   CS   3      ShutterShape
0018   1602   IS   1      ShutterLeftVerticalEdge
0018   1604   IS   1      ShutterRightVerticalEdge
0018   1606   IS   1      ShutterUpperHorizontalEdge
0018   1608   IS   1      ShutterLowerHorizontalEdge
0018   1610   IS   1      CenterOfCircularShutter
0018   1612   IS   1      RadiusOfCircularShutter
0018   1620   IS   1-n    VerticesOfPolygonalShutter
0018   1622   US   1      ShutterPresentationValue
0018   1623   US   1      ShutterOverlayGroup
0018   1700   CS   3      CollimatorShape
0018   1702   IS   1      CollimatorLeftVerticalEdge
0018   1704   IS   1      CollimatorRightVerticalEdge
0018   1706   IS   1      CollimatorUpperHorizontalEdge
0018   1708   IS   1      CollimatorLowerHorizontalEdge
0018   1710   IS   1      CenterOfCircularCollimator
0018   1712   IS   1      RadiusOfCircularCollimator
0018   1720   IS   1-n    VerticesOfPolygonalCollimator
0018   1800   CS   1      AcquisitionTimeSynchronized
0018   1801   SH   1      TimeSource
0018   1802   CS   1      TimeDistributionProtocol
0018   1810   DT   1      AcquisitionTimestamp
0018   4000   LT   1-n    AcquisitionComments
0018   5000   SH   1-n    OutputPower
0018   5010   LO   3      TransducerData
0018   5012   DS   1      FocusDepth
0018   5020   LO   1      PreprocessingFunction
0018   5021   LO   1      PostprocessingFunction
0018   5022   DS   1      MechanicalIndex
0018   5024   DS   1      ThermalIndex
0018   5026   DS   1      CranialThermalIndex
0018   5027   DS   1      SoftTissueThermalIndex
0018   5028   DS   1      SoftTissueFocusThermalIndex
0018   5029   DS   1      SoftTissueSurfaceThermalIndex
0018   5030   DS   1      DynamicRange
0018   5040   DS   1      TotalGain
0018   5050   IS   1      DepthOfScanField
0018   5100   CS   1      PatientPosition
0018   5101   CS   1      ViewPosition
0018   5104   SQ   1      ProjectionEponymousNameCodeSequence
0018   5210   DS   6      ImageTransformationMatrix
0018   5212   DS   3      ImageTranslationVector
0018   6000   DS   1      Sensitivity
0018   6011   SQ   1      SequenceOfUltrasoundRegions
0018   6012   US   1      RegionSpatialFormat
0018   6014   US   1      RegionDataType
0018   6016   UL   1      RegionFlags
0018   6018   UL   1      RegionLocationMinX0
0018   601A   UL   1      RegionLocationMinY0
0018   601C   UL   1      RegionLocationMaxX1
0018   601E   UL   1      RegionLocationMaxY1
0018   6020   SL   1      ReferencePixelX0
0018   6022   SL   1      ReferencePixelY0
0018   6024   US   1      PhysicalUnitsXDirection
0018   6026   US   1      PhysicalUnitsYDirection
0018   6028   FD   1      ReferencePixelPhysicalValueX
0018   602A   FD   1      ReferencePixelPhysicalValueY
0018   602C   FD   1      PhysicalDeltaX
0018   602E   FD   1      PhysicalDeltaY
0018   6030   UL   1      TransducerFrequency
0018   6031   CS   1      TransducerType
0018   6032   UL   1      PulseRepetitionFrequency
0018   6034   FD   1      DopplerCorrectionAngle
0018   6036   FD   1      SteeringAngle
0018   6038   UL   1      DopplerSampleVolumeXPosition
0018   603A   UL   1      DopplerSampleVolumeYPosition
0018   603C   UL   1      TMLinePositionX0
0018   603E   UL   1      TMLinePositionY0
0018   6040   UL   1      TMLinePositionX1
0018   6042   UL   1      TMLinePositionY1
0018   6044   US   1      PixelComponentOrganization
0018   6046   UL   1      PixelComponentMask
0018   6048   UL   1      PixelComponentRangeStart
0018   604A   UL   1      PixelComponentRangeStop
0018   604C   US   1      PixelComponentPhysicalUnits
0018   604E   US   1      PixelComponentDataType
0018   6050   UL   1      NumberOfTableBreakPoints
0018   6052   UL   1-n    TableOfXBreakPoints
0018   6054   FD   1-n    TableOfYBreakPoints
0018   6056   UL   1      NumberOfTableEntries
0018   6058   UL   1-n    TableOfPixelValues
0018   605A   FL   1-n    TableOfParameterValues
0018   7000   CS   1      DetectorConditionsNominalFlag
0018   7001   DS   1      DetectorTemperature
0018   7004   CS   1      DetectorType
0018   7005   CS   1      DetectorConfiguration
0018   7006   LT   1      DetectorDescription
0018   7008   LT   1      DetectorMode
0018   700A   SH   1      DetectorID
0018   700C   DA   1      DateofLastDetectorCalibration
0018   700E   TM   1      TimeofLastDetectorCalibration
0018   7010   IS   1      ExposuresOnDetectorSinceLastCalibration
0018   7011   IS   1      ExposuresOnDetectorSinceManufactured
0018   7012   DS   1      DetectorTimeSinceLastExposure
0018   7014   DS   1      DetectorActiveTime
0018   7016   DS   1      DetectorActivationOffsetFromExposure
0018   701A   DS   2      DetectorBinning
0018   7020   DS   2      DetectorElementPhysicalSize
0018   7022   DS   2      DetectorElementSpacing
0018   7024   CS   1      DetectorActiveShape
0018   7026   DS   1-2    DetectorActiveDimensions
0018   7028   DS   2      DetectorActiveOrigin
0018   7030   DS   2      FieldofViewOrigin
0018   7032   DS   1      FieldofViewRotation
0018   7034   CS   1      FieldofViewHorizontalFlip
0018   7040   LT   1      GridAbsorbingMaterial
0018   7041   LT   1      GridSpacingMaterial
0018   7042   DS   1      GridThickness
0018   7044   DS   1      GridPitch
0018   7046   IS   2      GridAspectRatio
0018   7048   DS   1      GridPeriod
0018   704C   DS   1      GridFocalDistance
0018   7050   LT   1-n    FilterMaterial
0018   7052   DS   1-n    FilterThicknessMinimum
0018   7054   DS   1-n    FilterThicknessMaximum
0018   7060   CS   1      ExposureControlMode
0018   7062   LT   1      ExposureControlModeDescription
0018   7064   CS   1      ExposureStatus
0018   7065   DS   1      PhototimerSetting
0020   0000   UL   1      ImageGroupLength
0020   000D   UI   1      StudyInstanceUID
0020   000E   UI   1      SeriesInstanceUID
0020   0010   SH   1      StudyID
0020   0011   IS   1      SeriesNumber
0020   0012   IS   1      AcquisitionNumber
0020   0013   IS   1      InstanceNumber
0020   0014   IS   1      IsotopeNumber
0020   0015   IS   1      PhaseNumber
0020   0016   IS   1      IntervalNumber
0020   0017   IS   1      TimeSlotNumber
0020   0018   IS   1      AngleNumber
0020   0019   IS   1      ItemNumber
0020   0020   CS   2      PatientOrientation
0020   0022   IS   1      OverlayNumber
0020   0024   IS   1      CurveNumber
0020   0026   IS   1      LUTNumber
0020   0030   DS   3      ImagePosition
0020   0032   DS   3      ImagePositionPatient
0020   0035   DS   6      ImageOrientation
0020   0037   DS   6      ImageOrientationPatient
0020   0050   DS   1      Location
0020   0052   UI   1      FrameOfReferenceUID
0020   0060   CS   1      Laterality
0020   0062   CS   1      ImageLaterality
0020   0070   LT   1      ImageGeometryType
0020   0080   CS   1-n    MaskingImage
0020   0100   IS   1      TemporalPositionIdentifier
0020   0105   IS   1      NumberOfTemporalPositions
0020   0200   UI   1      SynchronizationFrameofReferenceUID
0020   0110   DS   1      TemporalResolution
0020   1000   IS   1      SeriesInStudy
0020   1001   IS   1      AcquisitionsInSeries
0020   1002   IS   1      ImagesInAcquisition
0020   1003   IS   1      ImagesInSeries
0020   1004   IS   1      AcquisitionsInStudy
0020   1005   IS   1      ImagesInStudy
0020   1020   CS   1-n    Reference
0020   1040   LO   1      PositionReferenceIndicator
0020   1041   DS   1      SliceLocation
0020   1070   IS   1-n    OtherStudyNumbers
0020   1200   IS   1      NumberOfPatientRelatedStudies
0020   1202   IS   1      NumberOfPatientRelatedSeries
0020   1204   IS   1      NumberOfPatientRelatedImages
0020   1206   IS   1      NumberOfStudyRelatedSeries
0020   1208   IS   1      NumberOfStudyRelatedImages
0020   1209   IS   1      NumberOfSeriesRelatedInstances
0020   3100   CS   1-n    SourceImageID
0020   3401   CS   1      ModifyingDeviceID
0020   3402   CS   1      ModifiedImageID
0020   3403   DA   1      ModifiedImageDate
0020   3404   LO   1      ModifyingDeviceManufacturer
0020   3405   TM   1      ModifiedImageTime
0020   3406   LT   1      ModifiedImageDescription
0020   4000   LT   1      ImageComments
0020   5000   AT   1-n    OriginalImageIdentification
0020   5002   CS   1-n    OriginalImageIdentificationNomenclature
0028   0000   UL   1      ImagePresentationGroupLength
0028   0002   US   1      SamplesPerPixel
0028   0004   CS   1      PhotometricInterpretation
0028   0005   US   1      ImageDimensions
0028   0006   US   1      PlanarConfiguration
0028   0008   IS   1      NumberOfFrames
0028   0009   AT   1      FrameIncrementPointer
0028   0010   US   1      Rows
0028   0011   US   1      Columns
0028   0012   US   1      Planes
0028   0014   US   1      UltrasoundColorDataPresent
0028   0030   DS   2      PixelSpacing
0028   0031   DS   2      ZoomFactor
0028   0032   DS   2      ZoomCenter
0028   0034   IS   2      PixelAspectRatio
0028   0040   CS   1      ImageFormat
0028   0050   LT   1-n    ManipulatedImage
0028   0051   CS   1      CorrectedImage
0028   005F   CS   1      CompressionRecognitionCode
0028   0060   CS   1      CompressionCode
0028   0061   SH   1      CompressionOriginator
0028   0062   SH   1      CompressionLabel
0028   0063   SH   1      CompressionDescription
0028   0065   CS   1-n    CompressionSequence
0028   0066   AT   1-n    CompressionStepPointers
0028   0068   US   1      RepeatInterval
0028   0069   US   1      BitsGrouped
0028   0070   US   1-n    PerimeterTable
0028   0071   US/SS   1      PerimeterValue
0028   0080   US   1      PredictorRows
0028   0081   US   1      PredictorColumns
0028   0082   US   1-n    PredictorConstants
0028   0090   CS   1      BlockedPixels
0028   0091   US   1      BlockRows
0028   0092   US   1      BlockColumns
0028   0093   US   1      RowOverlap
0028   0094   US   1      ColumnOverlap
0028   0100   US   1      BitsAllocated
0028   0101   US   1      BitsStored
0028   0102   US   1      HighBit
0028   0103   US   1      PixelRepresentation
0028   0104   US   1      SmallestValidPixelValue
0028   0105   US   1      LargestValidPixelValue
0028   0106   US   1      SmallestImagePixelValue
0028   0107   US   1      LargestImagePixelValue
0028   0108   US   1      SmallestPixelValueInSeries
0028   0109   US   1      LargestPixelValueInSeries
0028   0110   US   1      SmallestPixelValueInPlane
0028   0111   US   1      LargestPixelValueInPlane
0028   0120   US   1      PixelPaddingValue
0028   0200   US   1      ImageLocation
0028   0300   CS   1      QualityControlImage
0028   0301   CS   1      BurnedInAnnotation
0028   0400   CS   1      TransformLabel
0028   0401   CS   1      TransformVersionNumber
0028   0402   US   1      NumberOfTransformSteps
0028   0403   CS   1-n    SequenceOfCompressedData
0028   0404   AT   1-n    DetailsOfCoefficients
0028   0410   US   1      RowsForNthOrderCoefficients
0028   0411   US   1      ColumnsForNthOrderCoefficients
0028   0412   CS   1-n    CoefficientCoding
0028   0413   AT   1-n    CoefficientCodingPointers
0028   0700   CS   1      DCTLabel
0028   0701   CS   1-n    DataBlockDescription
0028   0702   AT   1-n    DataBlock
0028   0710   US   1      NormalizationFactorFormat
0028   0720   US   1      ZonalMapNumberFormat
0028   0721   AT   1-n    ZonalMapLocation
0028   0722   US   1      ZonalMapFormat
0028   0730   US   1      AdaptiveMapFormat
0028   0740   US   1      CodeNumberFormat
0028   0800   CS   1-n    CodeLabel
0028   0802   US   1      NumberOfTables
0028   0803   AT   1-n    CodeTableLocation
0028   0804   US   1      BitsForCodeWord
0028   0808   AT   1-n    ImageDataLocation
0028   1040   CS   1      PixelIntensityRelationship
0028   1041   SS   1      PixelIntensityRelationshipSign
0028   1050   DS   1-n    WindowCenter
0028   1051   DS   1-n    WindowWidth
0028   1052   DS   1      RescaleIntercept
0028   1053   DS   1      RescaleSlope
0028   1054   LO   1      RescaleType
0028   1055   LO   1-n    WindowCenterWidthExplanation
0028   1080   CS   1      GrayScale
0028   1090   CS   1      RecommendedViewingMode
0028   1100   US/SS   3      GrayLookupTableDescriptor
0028   1101   US/SS   3      RedPaletteColorLookupTableDescriptor
0028   1102   US/SS   3      GreenPaletteColorLookupTableDescriptor
0028   1103   US/SS   3      BluePaletteColorLookupTableDescriptor
0028   1111   US   4      LargeRedPaletteColorLookupTableDescriptor
0028   1112   US   4      LargeGreenPaletteColorLookupTabe
0028   1113   US   4      LargeBluePaletteColorLookupTabl
0028   1199   UI   1      PaletteColorLookupTableUID
0028   1200   US/SS   1-n    GrayLookupTableData
0028   1201   US/SS   1-n    RedPaletteColorLookupTableData
0028   1202   US/SS   1-n    GreenPaletteColorLookupTableData
0028   1203   US/SS   1-n    BluePaletteColorLookupTableData
0028   1211   OW   1      LargeRedPaletteColorLookupTableData
0028   1212   OW   1      LargeGreenPaletteColorLookupTableData
0028   1213   OW   1      LargeBluePaletteColorLookupTableData
0028   1214   UI   1      LargePaletteColorLookupTableUID
0028   1221   OW   1      SegmentedRedPaletteColorLookupTableData
0028   1222   OW   1      SegmentedGreenPaletteColorLookupTableData
0028   1223   OW   1      SegmentedBluePaletteColorLookupTableData
0028   1300   CS   1      ImplantPresent
0028   2110   CS   1      LossyImageCompression
0028   2112   DS   1-n    LossyImageCompressionRatio
0028   3000   SQ   1      ModalityLUTSequence
0028   3002   US/SS   3      LUTDescriptor
0028   3003   LO   1      LUTExplanation
0028   3004   LO   1      ModalityLUTType
0028   3006   US/SS   1-n    LUTData
0028   3010   SQ   1      VOILUTSequence
0028   3110   SQ   1      SoftcopyVOILUTSequence
0028   4000   LT   1-n    ImagePresentationComments
0028   5000   SQ   1      BiPlaneAcquisitionSequence
0028   6010   US   1      RepresentativeFrameNumber
0028   6020   US   1-n    FrameNumbersOfInterest
0028   6022   LO   1-n    FrameOfInterestDescription
0028   6030   US   1-n    MaskPointer
0028   6040   US   1-n    RWavePointer
0028   6100   SQ   1      MaskSubtractionSequence
0028   6101   CS   1      MaskOperation
0028   6102   US   1-n    ApplicableFrameRange
0028   6110   US   1-n    MaskFrameNumbers
0028   6112   US   1      ContrastFrameAveraging
0028   6114   FL   2      MaskSubPixelShift
0028   6120   SS   1      TIDOffset
0028   6190   ST   1      MaskOperationExplanation
0032   0000   UL   1      StudyGroupLength
0032   000A   CS   1      StudyStatusID
0032   000C   CS   1      StudyPriorityID
0032   0012   LO   1      StudyIDIssuer
0032   0032   DA   1      StudyVerifiedDate
0032   0033   TM   1      StudyVerifiedTime
0032   0034   DA   1      StudyReadDate
0032   0035   TM   1      StudyReadTime
0032   1000   DA   1      ScheduledStudyStartDate
0032   1001   TM   1      ScheduledStudyStartTime
0032   1010   DA   1      ScheduledStudyStopDate
0032   1011   TM   1      ScheduledStudyStopTime
0032   1020   LO   1      ScheduledStudyLocation
0032   1021   AE   1-n    ScheduledStudyLocationAETitle
0032   1030   LO   1      ReasonForStudy
0032   1032   PN   1      RequestingPhysician
0032   1033   LO   1      RequestingService
0032   1040   DA   1      StudyArrivalDate
0032   1041   TM   1      StudyArrivalTime
0032   1050   DA   1      StudyCompletionDate
0032   1051   TM   1      StudyCompletionTime
0032   1055   CS   1      StudyComponentStatusID
0032   1060   LO   1      RequestedProcedureDescription
0032   1064   SQ   1      RequestedProcedureCodeSequence
0032   1070   LO   1      RequestedContrastAgent
0032   4000   LT   1      StudyComments
0038   0000   UL   1      VisitGroupLength
0038   0004   SQ   1      ReferencedPatientAliasSequence
0038   0008   CS   1      VisitStatusID
0038   0010   LO   1      AdmissionID
0038   0011   LO   1      IssuerOfAdmissionID
0038   0016   LO   1      RouteOfAdmissions
0038   001A   DA   1      ScheduledAdmissionDate
0038   001B   TM   1      ScheduledAdmissionTime
0038   001C   DA   1      ScheduledDischargeDate
0038   001D   TM   1      ScheduledDischargeTime
0038   001E   LO   1      ScheduledPatientInstitutionResidence
0038   0020   DA   1      AdmittingDate
0038   0021   TM   1      AdmittingTime
0038   0030   DA   1      DischargeDate
0038   0032   TM   1      DischargeTime
0038   0040   LO   1      DischargeDiagnosisDescription
0038   0044   SQ   1      DischargeDiagnosisCodeSequence
0038   0050   LO   1      SpecialNeeds
0038   0300   LO   1      CurrentPatientLocation
0038   0400   LO   1      PatientInstitutionResidence
0038   0500   LO   1      PatientState
0038   4000   LT   1      VisitComments
003A   0004   CS   1      WaveformOriginality
003A   0005   US   1      NumberofChannels
003A   0010   UL   1      NumberofSamples
003A   001A   DS   1      SamplingFrequency
003A   0020   SH   1      MultiplexGroupLabel
003A   0200   SQ   1      ChannelDefinitionSequence
003A   0202   IS   1      WVChannelNumber
003A   0203   SH   1      ChannelLabel
003A   0205   CS   1-n    ChannelStatus
003A   0208   SQ   1      ChannelSourceSequence
003A   0209   SQ   1      ChannelSourceModifiersSequence
003A   020A   SQ   1      SourceWaveformSequence
003A   020C   LO   1      ChannelDerivationDescription
003A   0210   DS   1      ChannelSensitivity
003A   0211   SQ   1      ChannelSensitivityUnits
003A   0212   DS   1      ChannelSensitivityCorrectionFactor
003A   0213   DS   1      ChannelBaseline
003A   0214   DS   1      ChannelTimeSkew
003A   0215   DS   1      ChannelSampleSkew
003A   0218   DS   1      ChannelOffset
003A   021A   US   1      WaveformBitsStored
003A   0220   DS   1      FilterLowFrequency
003A   0221   DS   1      FilterHighFrequency
003A   0222   DS   1      NotchFilterFrequency
003A   0223   DS   1      NotchFilterBandwidth
0040   0000   UL   1      ModalityWorklistGroupLength
0040   0001   AE   1      ScheduledStationAETitle
0040   0002   DA   1      ScheduledProcedureStepStartDate
0040   0003   TM   1      ScheduledProcedureStepStartTime
0040   0004   DA   1      ScheduledProcedureStepEndDate
0040   0005   TM   1      ScheduledProcedureStepEndTime
0040   0006   PN   1      ScheduledPerformingPhysicianName
0040   0007   LO   1      ScheduledProcedureStepDescription
0040   0008   SQ   1      ScheduledProcedureStepCodeSequence
0040   0009   SH   1      ScheduledProcedureStepID
0040   0010   SH   1      ScheduledStationName
0040   0011   SH   1      ScheduledProcedureStepLocation
0040   0012   LO   1      ScheduledPreOrderOfMedication
0040   0020   CS   1      ScheduledProcedureStepStatus
0040   0100   SQ   1-n    ScheduledProcedureStepSequence
0040   0220   SQ   1      ReferencedStandaloneSOPInstanceSequence
0040   0241   AE   1      PerformedStationAETitle
0040   0242   SH   1      PerformedStationName
0040   0243   SH   1      PerformedLocation
0040   0244   DA   1      PerformedProcedureStepStartDate
0040   0245   TM   1      PerformedProcedureStepStartTime
0040   0250   DA   1      PerformedProcedureStepEndDate
0040   0251   TM   1      PerformedProcedureStepEndTime
0040   0252   CS   1      PerformedProcedureStepStatus
0040   0253   CS   1      PerformedProcedureStepID
0040   0254   LO   1      PerformedProcedureStepDescription
0040   0255   LO   1      PerformedProcedureTypeDescription
0040   0260   SQ   1      PerformedActionItemSequence
0040   0270   SQ   1      ScheduledStepAttributesSequence
0040   0275   SQ   1      RequestAttributesSequence
0040   0280   ST   1      CommentsOnThePerformedProcedureSteps
0040   0293   SQ   1      QuantitySequence
0040   0294   DS   1      Quantity
0040   0295   SQ   1      MeasuringUnitsSequence
0040   0296   SQ   1      BillingItemSequence
0040   0300   US   1      TotalTimeOfFluoroscopy
0040   0301   US   1      TotalNumberOfExposures
0040   0302   US   1      EntranceDose
0040   0303   US   1-2    ExposedArea
0040   0306   DS   1      DistanceSourceToEntrance
0040   0307   DS   1      DistanceSourceToSupport
0040   0310   ST   1      CommentsOnRadiationDose
0040   0312   DS   1      XRayOutput
0040   0314   DS   1      HalfValueLayer
0040   0316   DS   1      OrganDose
0040   0318   CS   1      OrganExposed
0040   0320   SQ   1      BillingProcedureStepSequence
0040   0321   SQ   1      FilmConsumptionSequence
0040   0324   SQ   1      BillingSuppliesAndDevicesSequence
0040   0330   SQ   1      ReferencedProcedureStepSequence
0040   0340   SQ   1      PerformedSeriesSequence
0040   0400   LT   1      CommentsOnScheduledProcedureStep
0040   050A   LO   1      SpecimenAccessionNumber
0040   0550   SQ   1      SpecimenSequence
0040   0551   LO   1      SpecimenIdentifier
0040   0555   SQ   1      AcquisitionContextSequence
0040   0556   ST   1      AcquisitionContextDescription
0040   059A   SQ   1      SpecimenTypeCodeSequence
0040   06FA   LO   1      SlideIdentifier
0040   071A   SQ   1      ImageCenterPointCoordinatesSequence
0040   072A   DS   1      XOffsetInSlideCoordinateSystem
0040   073A   DS   1      YOffsetInSlideCoordinateSystem
0040   074A   DS   1      ZOffsetInSlideCoordinateSystem
0040   08D8   SQ   1      PixelSpacingSequence
0040   08DA   SQ   1      CoordinateSystemAxisCodeSequence
0040   08EA   SQ   1      MeasurementUnitsCodeSequence
0040   1001   SH   1      RequestedProcedureID
0040   1002   LO   1      ReasonForRequestedProcedure
0040   1003   SH   1      RequestedProcedurePriority
0040   1004   LO   1      PatientTransportArrangements
0040   1005   LO   1      RequestedProcedureLocation
0040   1006   SH   1      PlacerOrderNumberOfProcedure
0040   1007   SH   1      FillerOrderNumberOfProcedure
0040   1008   LO   1      ConfidentialityCode
0040   1009   SH   1      ReportingPriority
0040   1010   PN   1-n    NamesOfIntendedRecipientsOfResults
0040   1400   LT   1      RequestedProcedureComments
0040   2001   LO   1      ReasonForTheImagingServiceRequest
0040   2002   LO   1      ImagingServiceRequestDescription
0040   2004   DA   1      IssueDateOfImagingServiceRequest
0040   2005   TM   1      IssueTimeOfImagingServiceRequest
0040   2006   SH   1      PlacerOrderNumberOfImagingServiceRequest
0040   2007   SH   0      FillerOrderNumberOfImagingServiceRequest
0040   2008   PN   1      OrderEnteredBy
0040   2009   SH   1      OrderEntererLocation
0040   2010   SH   1      OrderCallbackPhoneNumber
0040   2016   LO   1      PlacerOrderNumberImagingServiceRequest
0040   2017   LO   1      FillerOrderNumberImagingServiceRequest
0040   2400   LT   1      ImagingServiceRequestComments
0040   3001   LT   1      ConfidentialityConstraint
0040   A010   CS   1      RelationshipType
0040   A027   LO   1      VerifyingOrganization
0040   A030   DT   1      VerificationDateTime
0040   A032   DT   1      ObservationDateTime
0040   A040   CS   1      ValueType
0040   A043   SQ   1      ConceptNameCodeSequence
0040   A050   CS   1      ContinuityOfContent
0040   A073   SQ   1      VerifyingObserverSequence
0040   A075   PN   1      VerifyingObserverName
0040   A088   SQ   1      VerifyingObserverIdentificationCodeSeque
0040   A0B0   US   2-2n   ReferencedWaveformChannels
0040   A120   DT   1      DateTime
0040   A121   DA   1      Date
0040   A122   TM   1      Time
0040   A123   PN   1      PersonName
0040   A124   UI   1      UID
0040   A130   CS   1      TemporalRangeType
0040   A132   UL   1-n    ReferencedSamplePositionsU
0040   A136   US   1-n    ReferencedFrameNumbers
0040   A138   DS   1-n    ReferencedTimeOffsets
0040   A13A   DT   1-n    ReferencedDatetime
0040   A160   UT   1      TextValue
0040   A168   SQ   1      ConceptCodeSequence
0040   A180   US   1      AnnotationGroupNumber
0040   A195   SQ   1      ConceptNameCodeSequenceModifier
0040   A300   SQ   1      MeasuredValueSequence
0040   A30A   DS   1-n    NumericValue
0040   A360   SQ   1      PredecessorDocumentsSequence
0040   A370   SQ   1      ReferencedRequestSequence
0040   A372   SQ   1      PerformedProcedureCodeSequence
0040   A375   SQ   1      CurrentRequestedProcedureEvidenceSequenSequence
0040   A385   SQ   1      PertinentOtherEvidenceSequence
0040   A491   CS   1      CompletionFlag
0040   A492   LO   1      CompletionFlagDescription
0040   A493   CS   1      VerificationFlag
0040   A504   SQ   1      ContentTemplateSequence
0040   A525   SQ   1      IdenticalDocumentsSequence
0040   A730   SQ   1      ContentSequence
0040   B020   SQ   1      AnnotationSequence
0040   DB00   CS   1      TemplateIdentifier
0040   DB06   DT   1      TemplateVersion
0040   DB07   DT   1      TemplateLocalVersion
0040   DB0B   CS   1      TemplateExtensionFlag
0040   DB0C   UI   1      TemplateExtensionOrganizationUID
0040   DB0D   UI   1      TemplateExtensionCreatorUID
0040   DB73   UL   1-n    ReferencedContentItemIdentifier
0050   0000   UL   1      XRayAngioDeviceGroupLength
0050   0004   CS   1      CalibrationObject
0050   0010   SQ   1      DeviceSequence
0050   0012   CS   1      DeviceType
0050   0014   DS   1      DeviceLength
0050   0016   DS   1      DeviceDiameter
0050   0017   CS   1      DeviceDiameterUnits
0050   0018   DS   1      DeviceVolume
0050   0019   DS   1      InterMarkerDistance
0050   0020   LO   1      DeviceDescription
0050   0030   SQ   1      CodedInterventionalDeviceSequence
0054   0000   UL   1      NuclearMedicineGroupLength
0054   0010   US   1-n    EnergyWindowVector
0054   0011   US   1      NumberOfEnergyWindows
0054   0012   SQ   1      EnergyWindowInformationSequence
0054   0013   SQ   1      EnergyWindowRangeSequence
0054   0014   DS   1      EnergyWindowLowerLimit
0054   0015   DS   1      EnergyWindowUpperLimit
0054   0016   SQ   1      RadiopharmaceuticalInformationSequence
0054   0017   IS   1      ResidualSyringeCounts
0054   0018   SH   1      EnergyWindowName
0054   0020   US   1-n    DetectorVector
0054   0021   US   1      NumberOfDetectors
0054   0022   SQ   1      DetectorInformationSequence
0054   0030   US   1-n    PhaseVector
0054   0031   US   1      NumberOfPhases
0054   0032   SQ   1      PhaseInformationSequence
0054   0033   US   1      NumberOfFramesInPhase
0054   0036   IS   1      PhaseDelay
0054   0038   IS   1      PauseBetweenFrames
0054   0050   US   1-n    RotationVector
0054   0051   US   1      NumberOfRotations
0054   0052   SQ   1      RotationInformationSequence
0054   0053   US   1      NumberOfFramesInRotation
0054   0060   US   1-n    RRIntervalVector
0054   0061   US   1      NumberOfRRIntervals
0054   0062   SQ   1      GatedInformationSequence
0054   0063   SQ   1      DataInformationSequence
0054   0070   US   1-n    TimeSlotVector
0054   0071   US   1      NumberOfTimeSlots
0054   0072   SQ   1      TimeSlotInformationSequence
0054   0073   DS   1      TimeSlotTime
0054   0080   US   1-n    SliceVector
0054   0081   US   1      NumberOfSlices
0054   0090   US   1-n    AngularViewVector
0054   0100   US   1-n    TimeSliceVector
0054   0101   US   1      NumberOfTimeSlices
0054   0200   DS   1      StartAngle
0054   0202   CS   1      TypeOfDetectorMotion
0054   0210   IS   1-n    TriggerVector
0054   0211   US   1      NumberOfTriggersInPhase
0054   0220   SQ   1      ViewCodeSequence
0054   0222   SQ   1      ViewAngulationModifierCodeSequence
0054   0300   SQ   1      RadionuclideCodeSequence
0054   0302   SQ   1      AdministrationRouteCodeSequence
0054   0304   SQ   1      RadiopharmaceuticalCodeSequence
0054   0306   SQ   1      CalibrationDataSequence
0054   0308   US   1      EnergyWindowNumber
0054   0400   SH   1      ImageID
0054   0410   SQ   1      PatientOrientationCodeSequence
0054   0412   SQ   1      PatientOrientationModifierCodeSequence
0054   0414   SQ   1      PatientGantryRelationshipCodeSequence
0054   1000   CS   2      SeriesType
0054   1001   CS   1      Units
0054   1002   CS   1      CountsSource
0054   1004   CS   1      ReprojectionMethod
0054   1100   CS   1      RandomsCorrectionMethod
0054   1101   LO   1      AttenuationCorrectionMethod
0054   1102   CS   1      DecayCorrection
0054   1103   LO   1      ReconstructionMethod
0054   1104   LO   1      DetectorLinesOfResponseUsed
0054   1105   LO   1      ScatterCorrectionMethod
0054   1200   DS   1      AxialAcceptance
0054   1201   IS   2      AxialMash
0054   1202   IS   1      TransverseMash
0054   1203   DS   2      DetectorElementSize
0054   1210   DS   1      CoincidenceWindowWidth
0054   1220   CS   1-n    SecondaryCountsType
0054   1300   DS   1      FrameReferenceTime
0054   1310   IS   1      PrimaryPromptsCountsAccumulated
0054   1311   IS   1-n    SecondaryCountsAccumulated
0054   1320   DS   1      SliceSensitivityFactor
0054   1321   DS   1      DecayFactor
0054   1322   DS   1      DoseCalibrationFactor
0054   1323   DS   1      ScatterFractionFactor
0054   1324   DS   1      DeadTimeFactor
0054   1330   US   1      ImageIndex
0054   1400   CS   1-n    CountsIncluded
0054   1401   CS   1      DeadTimeCorrectionFlag
0060   0000   UL   1      HistogramGroupLength
0060   3000   SQ   1      HistogramSequence
0060   3002   US   1      HistogramNumberofBins
0060   3004   US/SS 1      HistogramFirstBinValue
0060   3006   US/SS 1      HistogramLastBinValue
0060   3008   US   1      HistogramBinWidth
0060   3010   LO   1      HistogramExplanation
0060   3020   UL   1-n    HistogramData
0070   0001   SQ   1      GraphicAnnotationSequence
0070   0002   CS   1      GraphicLayer
0070   0003   CS   1      BoundingBoxAnnotationUnits
0070   0004   CS   1      AnchorPointAnnotationUnits
0070   0005   CS   1      GraphicAnnotationUnits
0070   0006   ST   1      UnformattedTextValue
0070   0008   SQ   1      TextObjectSequence
0070   0009   SQ   1      GraphicObjectSequence
0070   0010   FL   2      BoundingBoxTopLeftHandCorner
0070   0011   FL   2      BoundingBoxBottomRightHandCorner
0070   0012   CS   1      BoundingBoxTextHorizontalJustification
0070   0014   FL   2      AnchorPoint
0070   0015   CS   1      AnchorPointVisibility
0070   0020   US   1      GraphicDimensions
0070   0021   US   1      NumberOfGraphicPoints
0070   0022   FL   2-n    GraphicData
0070   0023   CS   1      GraphicType
0070   0024   CS   1      GraphicFilled
0070   0040   IS   1      ImageRotationFrozenDraftRetired
0070   0041   CS   1      ImageHorizontalFlip
0070   0042   US   1      ImageRotation
0070   0050   US   2      DisplayedAreaTLHCFrozenDraftRetired
0070   0051   US   2      DisplayedAreaBRHCFrozenDraftRetired
0070   0052   SL   2      DisplayedAreaTopLeftHandCorner
0070   0053   SL   2      DisplayedAreaBottomRightHandCorner
0070   005A   SQ   1      DisplayedAreaSelectionSequence
0070   0060   SQ   1      GraphicLayerSequence
0070   0062   IS   1      GraphicLayerOrder
0070   0066   US   1      GraphicLayerRecommendedDisplayGrayscaleValue
0070   0067   US   3      GraphicLayerRecommendedDisplayRGBValue
0070   0068   LO   1      GraphicLayerDescription
0070   0080   CS   1      PresentationLabel
0070   0081   LO   1      PresentationDescription
0070   0082   DA   1      PresentationCreationDate
0070   0083   TM   1      PresentationCreationTime
0070   0084   PN   1      PresentationCreatorsName
0070   0100   CS   1      PresentationSizeMode
0070   0101   DS   2      PresentationPixelSpacing
0070   0102   IS   2      PresentationPixelAspectRatio
0070   0103   FL   1      PresentationPixelMagnificationRatio
0088   0000   UL   1      StorageGroupLength
0088   0130   SH   1      StorageMediaFilesetID
0088   0140   UI   1      StorageMediaFilesetUID
0088   0200   SQ   1      IconImage
0088   0904   LO   1      TopicTitle
0088   0906   ST   1      TopicSubject
0088   0910   LO   1      TopicAuthor
0088   0912   LO   3      TopicKeyWords
1000   0000   UL   1      CodeTableGroupLength
1000   0010   US   3      EscapeTriplet
1000   0011   US   3      RunLengthTriplet
1000   0012   US   1      HuffmanTableSize
1000   0013   US   3      HuffmanTableTriplet
1000   0014   US   1      ShiftTableSize
1000   0015   US   3      ShiftTableTriplet
1010   0000   UL   1      ZonalMapGroupLength
1010   0004   US   1-n    ZonalMap
2000   0000   UL   1      FilmSessionGroupLength
2000   0010   IS   1      NumberOfCopies
2000   001E   SQ   1      PrinterConfigurationSequence
2000   0020   CS   1      PrintPriority
2000   0030   CS   1      MediumType
2000   0040   CS   1      FilmDestination
2000   0050   LO   1      FilmSessionLabel
2000   0060   IS   1      MemoryAllocation
2000   0061   IS   1      MaximumMemoryAllocation
2000   0062   CS   1      ColorImagePrintingFlag
2000   0063   CS   1      CollationFlag
2000   0065   CS   1      AnnotationFlag
2000   0067   CS   1      ImageOverlayFlag
2000   0069   CS   1      PresentationLUTFlag
2000   006A   CS   1      ImageBoxPresentationLUTFlag
2000   00A0   US   1      MemoryBitDepth
2000   00A1   US   1      PrintingBitDepth
2000   00A2   SQ   1      MediaInstalledSequence
2000   00A4   SQ   1      OtherMediaAvailableSequence
2000   00A8   SQ   1      SupportedImageDisplayFormatsSequence
2000   0500   SQ   1      ReferencedFilmBoxSequence
2000   0510   SQ   1      ReferencedStoredPrintSequence
2010   0000   UL   1      FilmBoxGroupLength
2010   0010   ST   1      ImageDisplayFormat
2010   0030   CS   1      AnnotationDisplayFormatID
2010   0040   CS   1      FilmOrientation
2010   0050   CS   1      FilmSizeID
2010   0052   CS   1      PrinterResolutionID
2010   0054   CS   1      DefaultPrinterResolutionID
2010   0060   CS   1      MagnificationType
2010   0080   CS   1      SmoothingType
2010   00A6   CS   1      DefaultMagnificationType
2010   00A7   CS   1-n    OtherMagnificationTypesAvailable
2010   00A8   CS   1      DefaultSmoothingType
2010   00A9   CS   1-n    OtherSmoothingTypesAvailable
2010   0100   CS   1      BorderDensity
2010   0110   CS   1      EmptyImageDensity
2010   0120   US   1      MinDensity
2010   0130   US   1      MaxDensity
2010   0140   CS   1      Trim
2010   0150   ST   1      ConfigurationInformation
2010   0152   LT   1      ConfigurationInformationDescription
2010   0154   IS   1      MaximumCollatedFilms
2010   015E   US   1      Illumination
2010   0160   US   1      ReflectedAmbientLight
2010   0376   DS   2      PrinterPixelSpacing
2010   0500   SQ   1      ReferencedFilmSessionSequence
2010   0510   SQ   1      ReferencedImageBoxSequence
2010   0520   SQ   1      ReferencedBasicAnnotationBoxSequence
2020   0000   UL   1      ImageBoxGroupLength
2020   0010   US   1      ImageBoxPosition
2020   0020   CS   1      Polarity
2020   0030   DS   1      RequestedImageSize
2020   0040   CS   1      RequestedDecimateCropBehavior
2020   0050   CS   1      RequestedResolutionID
2020   00A0   CS   1      RequestedImageSizeFlag
2020   00A2   CS   1      DecimateCropResult
2020   0110   SQ   1      PreformattedGrayscaleImageSequence
2020   0111   SQ   1      PreformattedColorImageSequence
2020   0130   SQ   1      ReferencedImageOverlayBoxSequence
2020   0140   SQ   1      ReferencedVOILUTBoxSequence
2030   0000   UL   1      AnnotationGroupLength
2030   0010   US   1      AnnotationPosition
2030   0020   LO   1      TextString
2040   0000   UL   1      OverlayBoxGroupLength
2040   0010   SQ   1      ReferencedOverlayPlaneSequence
2040   0011   US   9      ReferencedOverlayPlaneGroups
2040   0020   SQ   1      OverlayPixelDataSequence
2040   0060   CS   1      OverlayMagnificationType
2040   0070   CS   1      OverlaySmoothingType
2040   0072   CS   1      OverlayOrImageMagnification
2040   0074   US   1      MagnifyToNumberOfColumns
2040   0080   CS   1      OverlayForegroundDensity
2040   0082   CS   1      OverlayBackgroundDensity
2040   0090   CS   1      OverlayMode
2040   0100   CS   1      ThresholdDensity
2040   0500   SQ   1      ReferencedOverlayImageBoxSequence
2050   0000   UL   1      PresentationLUTGroupLength
2050   0010   SQ   1      PresentationLUTSequence
2050   0020   CS   1      PresentationLUTShape
2050   0500   SQ   1      ReferencedPresentationLUTSequence
2100   0000   UL   1      PrintJobGroupLength
2100   0010   SH   1      PrintJobID
2100   0020   CS   1      ExecutionStatus
2100   0030   CS   1      ExecutionStatusInfo
2100   0040   DA   1      CreationDate
2100   0050   TM   1      CreationTime
2100   0070   AE   1      Originator
2100   0140   AE   1      DestinationAE
2100   0160   SH   1      OwnerID
2100   0170   IS   1      NumberOfFilms
2100   0500   SQ   1      ReferencedPrintJobSequence
2110   0000   UL   1      PrinterGroupLength
2110   0010   CS   1      PrinterStatus
2110   0020   CS   1      PrinterStatusInfo
2110   0030   LO   1      PrinterName
2110   0099   SH   1      PrintQueueID
2120   0000   UL   1      QueueGroupLength
2120   0010   CS   1      QueueStatus
2120   0050   SQ   1      PrintJobDescriptionSequence
2120   0070   SQ   1      QueueReferencedPrintJobSequence
2130   0000   UL   1      PrintContentGroupLength
2130   0010   SQ   1      PrintManagementCapabilitiesSequence
2130   0015   SQ   1      PrinterCharacteristicsSequence
2130   0030   SQ   1      FilmBoxContentSequence
2130   0040   SQ   1      ImageBoxContentSequence
2130   0050   SQ   1      AnnotationContentSequence
2130   0060   SQ   1      ImageOverlayBoxContentSequence
2130   0080   SQ   1      PresentationLUTContentSequence
2130   00A0   SQ   1      ProposedStudySequence
2130   00C0   SQ   1      OriginalImageSequence
3002   0000   UL   1      RTImageGroupLength
3002   0002   SH   1      RTImageLabel
3002   0003   LO   1      RTImageName
3002   0004   ST   1      RTImageDescription
3002   000A   CS   1      ReportedValuesOrigin
3002   000C   CS   1      RTImagePlane
3002   000D   DS   3      XRayImageReceptorTranslation
3002   000E   DS   1      XRayImageReceptorAngle
3002   0010   DS   6      RTImageOrientation
3002   0011   DS   2      ImagePlanePixelSpacing
3002   0012   DS   2      RTImagePosition
3002   0020   SH   1      RadiationMachineName
3002   0022   DS   1      RadiationMachineSAD
3002   0024   DS   1      RadiationMachineSSD
3002   0026   DS   1      RTImageSID
3002   0028   DS   1      SourceToReferenceObjectDistance
3002   0029   IS   1      FractionNumber
3002   0030   SQ   1      ExposureSequence
3002   0032   DS   1      MetersetExposure
3004   0000   UL   1      RTDoseGroupLength
3004   0001   CS   1      DVHType
3004   0002   CS   1      DoseUnits
3004   0004   CS   1      DoseType
3004   0006   LO   1      DoseComment
3004   0008   DS   3      NormalizationPoint
3004   000A   CS   1      DoseSummationType
3004   000C   DS   2-n    GridFrameOffsetVector
3004   000E   DS   1      DoseGridScaling
3004   0010   SQ   1      RTDoseROISequence
3004   0012   DS   1      DoseValue
3004   0040   DS   3      DVHNormalizationPoint
3004   0042   DS   1      DVHNormalizationDoseValue
3004   0050   SQ   1      DVHSequence
3004   0052   DS   1      DVHDoseScaling
3004   0054   CS   1      DVHVolumeUnits
3004   0056   IS   1      DVHNumberOfBins
3004   0058   DS   2-2n   DVHData
3004   0060   SQ   1      DVHReferencedROISequence
3004   0062   CS   1      DVHROIContributionType
3004   0070   DS   1      DVHMinimumDose
3004   0072   DS   1      DVHMaximumDose
3004   0074   DS   1      DVHMeanDose
3006   0000   UL   1      RTStructureSetGroupLength
3006   0002   SH   1      StructureSetLabel
3006   0004   LO   1      StructureSetName
3006   0006   ST   1      StructureSetDescription
3006   0008   DA   1      StructureSetDate
3006   0009   TM   1      StructureSetTime
3006   0010   SQ   1      ReferencedFrameOfReferenceSequence
3006   0012   SQ   1      RTReferencedStudySequence
3006   0014   SQ   1      RTReferencedSeriesSequence
3006   0016   SQ   1      ContourImageSequence
3006   0020   SQ   1      StructureSetROISequence
3006   0022   IS   1      ROINumber
3006   0024   UI   1      ReferencedFrameOfReferenceUID
3006   0026   LO   1      ROIName
3006   0028   ST   1      ROIDescription
3006   002A   IS   3      ROIDisplayColor
3006   002C   DS   1      ROIVolume
3006   0030   SQ   1      RTRelatedROISequence
3006   0033   CS   1      RTROIRelationship
3006   0036   CS   1      ROIGenerationAlgorithm
3006   0038   LO   1      ROIGenerationDescription
3006   0039   SQ   1      ROIContourSequence
3006   0040   SQ   1      ContourSequence
3006   0042   CS   1      ContourGeometricType
3006   0044   DS   1      ContourSlabThickness
3006   0045   DS   3      ContourOffsetVector
3006   0046   IS   1      NumberOfContourPoints
3006   0048   IS   1      ContourNumber
3006   0049   IS   1-n    AttachedContours
3006   0050   DS   3-3n   ContourData
3006   0080   SQ   1      RTROIObservationsSequence
3006   0082   IS   1      ObservationNumber
3006   0084   IS   1      ReferencedROINumber
3006   0085   SH   1      ROIObservationLabel
3006   0086   SQ   1      RTROIIdentificationCodeSequence
3006   0088   ST   1      ROIObservationDescription
3006   00A0   SQ   1      RelatedRTROIObservationsSequence
3006   00A4   CS   1      RTROIInterpretedType
3006   00A6   PN   1      ROIInterpreter
3006   00B0   SQ   1      ROIPhysicalPropertiesSequence
3006   00B2   CS   1      ROIPhysicalProperty
3006   00B4   DS   1      ROIPhysicalPropertyValue
3006   00C0   SQ   1      FrameOfReferenceRelationshipSequence
3006   00C2   UI   1      RelatedFrameOfReferenceUID
3006   00C4   CS   1      FrameOfReferenceTransformationType
3006   00C6   DS   16     FrameOfReferenceTransformationMatrix
3006   00C8   LO   1      FrameOfReferenceTransformationComment
3008   0010   SQ   1      MeasuredDoseReferenceSequence
3008   0012   ST   1      MeasuredDoseDescription
3008   0014   CS   1      MeasuredDoseType
3008   0016   DS   1      MeasuredDoseValue
3008   0020   SQ   1      TreatmentSessionBeamSequence
3008   0022   IS   1      CurrentFractionNumber
3008   0024   DA   1      TreatmentControlPointDate
3008   0025   TM   1      TreatmentControlPointTime
3008   002A   CS   1      TreatmentTerminationStatus
3008   002B   SH   1      TreatmentTerminationCode
3008   002C   CS   1      TreatmentVerificationStatus
3008   0030   SQ   1      ReferencedTreatmentRecordSequence
3008   0032   DS   1      SpecifiedPrimaryMeterset
3008   0033   DS   1      SpecifiedSecondaryMeterset
3008   0036   DS   1      DeliveredPrimaryMeterset
3008   0037   DS   1      DeliveredSecondaryMeterset
3008   003A   DS   1      SpecifiedTreatmentTime
3008   003B   DS   1      DeliveredTreatmentTime
3008   0040   SQ   1      ControlPointDeliverySequence
3008   0042   DS   1      SpecifiedMeterset
3008   0044   DS   1      DeliveredMeterset
3008   0048   DS   1      DoseRateDelivered
3008   0050   SQ   1      TreatmentSummaryCalculatedDoseReferenceSequence
3008   0052   DS   1      CumulativeDosetoDoseReference
3008   0054   DA   1      FirstTreatmentDate
3008   0056   DA   1      MostRecentTreatmentDate
3008   005A   IS   1      NumberofFractionsDelivered
3008   0060   SQ   1      OverrideSequence
3008   0062   AT   1      OverrideParameterPointer
3008   0064   IS   1      MeasuredDoseReferenceNumber
3008   0066   ST   1      OverrideReason
3008   0070   SQ   1      CalculatedDoseReferenceSequence
3008   0072   IS   1      CalculatedDoseReferenceNumber
3008   0074   ST   1      CalculatedDoseReferenceDescription
3008   0076   DS   1      CalculatedDoseReferenceDoseValue
3008   0078   DS   1      StartMeterset
3008   007A   DS   1      EndMeterset
3008   0080   SQ   1      ReferencedMeasuredDoseReferenceSequence
3008   0082   IS   1      ReferencedMeasuredDoseReferenceNumber
3008   0090   SQ   1      ReferencedCalculatedDoseReferenceSequence
3008   0092   IS   1      ReferencedCalculatedDoseReferenceNumber
3008   00A0   SQ   1      BeamLimitingDeviceLeafPairsSequence
3008   00B0   SQ   1      RecordedWedgeSequence
3008   00C0   SQ   1      RecordedCompensatorSequence
3008   00D0   SQ   1      RecordedBlockSequence
3008   00E0   SQ   1      TreatmentSummaryMeasuredDoseReferenceSequence
3008   0100   SQ   1      RecordedSourceSequence
3008   0105   LO   1      SourceSerialNumber
3008   0110   SQ   1      TreatmentSessionApplicationSetupSequence
3008   0116   CS   1      ApplicationSetupCheck
3008   0120   SQ   1      RecordedBrachyAccessoryDeviceSequence
3008   0122   IS   1      ReferencedBrachyAccessoryDeviceNumber
3008   0130   SQ   1      RecordedChannelSequence
3008   0132   DS   1      SpecifiedChannelTotalTime
3008   0134   DS   1      DeliveredChannelTotalTime
3008   0136   IS   1      SpecifiedNumberofPulses
3008   0138   IS   1      DeliveredNumberofPulses
3008   013A   DS   1      SpecifiedPulseRepetitionInterval
3008   013C   DS   1      DeliveredPulseRepetitionInterval
3008   0140   SQ   1      RecordedSourceApplicatorSequence
3008   0142   IS   1      ReferencedSourceApplicatorNumber
3008   0150   SQ   1      RecordedChannelShieldSequence
3008   0152   IS   1      ReferencedChannelShieldNumber
3008   0160   SQ   1      BrachyControlPointDeliveredSequence
3008   0162   DA   1      SafePositionExitDate
3008   0164   TM   1      SafePositionExitTime
3008   0166   DA   1      SafePositionReturnDate
3008   0168   TM   1      SafePositionReturnTime
3008   0200   CS   1      CurrentTreatmentStatus
3008   0202   ST   1      TreatmentStatusComment
3008   0220   SQ   1      FractionGroupSummarySequence
3008   0223   IS   1      ReferencedFractionNumber
3008   0224   CS   1      FractionGroupType
3008   0230   CS   1      BeamStopperPosition
3008   0240   SQ   1      FractionStatusSummarySequence
3008   0250   DA   1      TreatmentDate
3008   0251   TM   1      TreatmentTime
300A   0000   UL   1      RTPlanGroupLength
300A   0002   SH   1      RTPlanLabel
300A   0003   LO   1      RTPlanName
300A   0004   ST   1      RTPlanDescription
300A   0006   DA   1      RTPlanDate
300A   0007   TM   1      RTPlanTime
300A   0009   LO   1-n    TreatmentProtocols
300A   000A   CS   1      TreatmentIntent
300A   000B   LO   1-n    TreatmentSites
300A   000C   CS   1      RTPlanGeometry
300A   000E   ST   1      PrescriptionDescription
300A   0010   SQ   1      DoseReferenceSequence
300A   0012   IS   1      DoseReferenceNumber
300A   0014   CS   1      DoseReferenceStructureType
300A   0015   CS   1      NominalBeamEnergyUnit
300A   0016   LO   1      DoseReferenceDescription
300A   0018   DS   3      DoseReferencePointCoordinates
300A   001A   DS   1      NominalPriorDose
300A   0020   CS   1      DoseReferenceType
300A   0021   DS   1      ConstraintWeight
300A   0022   DS   1      DeliveryWarningDose
300A   0023   DS   1      DeliveryMaximumDose
300A   0025   DS   1      TargetMinimumDose
300A   0026   DS   1      TargetPrescriptionDose
300A   0027   DS   1      TargetMaximumDose
300A   0028   DS   1      TargetUnderdoseVolumeFraction
300A   002A   DS   1      OrganAtRiskFullVolumeDose
300A   002B   DS   1      OrganAtRiskLimitDose
300A   002C   DS   1      OrganAtRiskMaximumDose
300A   002D   DS   1      OrganAtRiskOverdoseVolumeFraction
300A   0040   SQ   1      ToleranceTableSequence
300A   0042   IS   1      ToleranceTableNumber
300A   0043   SH   1      ToleranceTableLabel
300A   0044   DS   1      GantryAngleTolerance
300A   0046   DS   1      BeamLimitingDeviceAngleTolerance
300A   0048   SQ   1      BeamLimitingDeviceToleranceSequence
300A   004A   DS   1      BeamLimitingDevicePositionTolerance
300A   004C   DS   1      PatientSupportAngleTolerance
300A   004E   DS   1      TableTopEccentricAngleTolerance
300A   0051   DS   1      TableTopVerticalPositionTolerance
300A   0052   DS   1      TableTopLongitudinalPositionTolerance
300A   0053   DS   1      TableTopLateralPositionTolerance
300A   0055   CS   1      RTPlanRelationship
300A   0070   SQ   1      FractionGroupSequence
300A   0071   IS   1      FractionGroupNumber
300A   0078   IS   1      NumberOfFractionsPlanned
300A   0079   IS   1      NumberOfFractionsPerDay
300A   007A   IS   1      RepeatFractionCycleLength
300A   007B   LT   1      FractionPattern
300A   0080   IS   1      NumberOfBeams
300A   0082   DS   3      BeamDoseSpecificationPoint
300A   0084   DS   1      BeamDose
300A   0086   DS   1      BeamMeterset
300A   00A0   IS   1      NumberOfBrachyApplicationSetups
300A   00A2   DS   3      BrachyApplicationSetupDoseSpecificationPoint
300A   00A4   DS   1      BrachyApplicationSetupDose
300A   00B0   SQ   1      BeamSequence
300A   00B2   SH   1      TreatmentMachineName
300A   00B3   CS   1      PrimaryDosimeterUnit
300A   00B4   DS   1      SourceAxisDistance
300A   00B6   SQ   1      BeamLimitingDeviceSequence
300A   00B8   CS   1      RTBeamLimitingDeviceType
300A   00BA   DS   1      SourceToBeamLimitingDeviceDistance
300A   00BC   IS   1      NumberOfLeafJawPairs
300A   00BE   DS   3-n    LeafPositionBoundaries
300A   00C0   IS   1      BeamNumber
300A   00C2   LO   1      BeamName
300A   00C3   ST   1      BeamDescription
300A   00C4   CS   1      BeamType
300A   00C6   CS   1      RadiationType
300A   00C8   IS   1      ReferenceImageNumber
300A   00CA   SQ   1      PlannedVerificationImageSequence
300A   00CC   LO   1-n    ImagingDeviceSpecificAcquisitionParameters
300A   00CE   CS   1      TreatmentDeliveryType
300A   00D0   IS   1      NumberOfWedges
300A   00D1   SQ   1      WedgeSequence
300A   00D2   IS   1      WedgeNumber
300A   00D3   CS   1      WedgeType
300A   00D4   SH   1      WedgeID
300A   00D5   IS   1      WedgeAngle
300A   00D6   DS   1      WedgeFactor
300A   00D8   DS   1      WedgeOrientation
300A   00DA   DS   1      SourceToWedgeTrayDistance
300A   00E0   IS   1      NumberOfCompensators
300A   00E1   SH   1      MaterialID
300A   00E2   DS   1      TotalCompensatorTrayFactor
300A   00E3   SQ   1      CompensatorSequence
300A   00E4   IS   1      CompensatorNumber
300A   00E5   SH   1      CompensatorID
300A   00E6   DS   1      SourceToCompensatorTrayDistance
300A   00E7   IS   1      CompensatorRows
300A   00E8   IS   1      CompensatorColumns
300A   00E9   DS   2      CompensatorPixelSpacing
300A   00EA   DS   2      CompensatorPosition
300A   00EB   DS   1-n    CompensatorTransmissionData
300A   00EC   DS   1-n    CompensatorThicknessData
300A   00ED   IS   1      NumberOfBoli
300A   00EE   CS   1      CompensatorType
300A   00F0   IS   1      NumberOfBlocks
300A   00F2   DS   1      TotalBlockTrayFactor
300A   00F4   SQ   1      BlockSequence
300A   00F5   SH   1      BlockTrayID
300A   00F6   DS   1      SourceToBlockTrayDistance
300A   00F8   CS   1      BlockType
300A   00FA   CS   1      BlockDivergence
300A   00FC   IS   1      BlockNumber
300A   00FE   LO   1      BlockName
300A   0100   DS   1      BlockThickness
300A   0102   DS   1      BlockTransmission
300A   0104   IS   1      BlockNumberOfPoints
300A   0106   DS   2-2n   BlockData
300A   0107   SQ   1      ApplicatorSequence
300A   0108   SH   1      ApplicatorID
300A   0109   CS   1      ApplicatorType
300A   010A   LO   1      ApplicatorDescription
300A   010C   DS   1      CumulativeDoseReferenceCoefficient
300A   010E   DS   1      FinalCumulativeMetersetWeight
300A   0110   IS   1      NumberOfControlPoints
300A   0111   SQ   1      ControlPointSequence
300A   0112   IS   1      ControlPointIndex
300A   0114   DS   1      NominalBeamEnergy
300A   0115   DS   1      DoseRateSet
300A   0116   SQ   1      WedgePositionSequence
300A   0118   CS   1      WedgePosition
300A   011A   SQ   1      BeamLimitingDevicePositionSequence
300A   011C   DS   2-2n   LeafJawPositions
300A   011E   DS   1      GantryAngle
300A   011F   CS   1      GantryRotationDirection
300A   0120   DS   1      BeamLimitingDeviceAngle
300A   0121   CS   1      BeamLimitingDeviceRotationDirection
300A   0122   DS   1      PatientSupportAngle
300A   0123   CS   1      PatientSupportRotationDirection
300A   0124   DS   1      TableTopEccentricAxisDistance
300A   0125   DS   1      TableTopEccentricAngle
300A   0126   CS   1      TableTopEccentricRotationDirection
300A   0128   DS   1      TableTopVerticalPosition
300A   0129   DS   1      TableTopLongitudinalPosition
300A   012A   DS   1      TableTopLateralPosition
300A   012C   DS   3      IsocenterPosition
300A   012E   DS   3      SurfaceEntryPoint
300A   0130   DS   1      SourceToSurfaceDistance
300A   0134   DS   1      CumulativeMetersetWeight
300A   0180   SQ   1      PatientSetupSequence
300A   0182   IS   1      PatientSetupNumber
300A   0184   LO   1      PatientAdditionalPosition
300A   0190   SQ   1      FixationDeviceSequence
300A   0192   CS   1      FixationDeviceType
300A   0194   SH   1      FixationDeviceLabel
300A   0196   ST   1      FixationDeviceDescription
300A   0198   SH   1      FixationDevicePosition
300A   01A0   SQ   1      ShieldingDeviceSequence
300A   01A2   CS   1      ShieldingDeviceType
300A   01A4   SH   1      ShieldingDeviceLabel
300A   01A6   ST   1      ShieldingDeviceDescription
300A   01A8   SH   1      ShieldingDevicePosition
300A   01B0   CS   1      SetupTechnique
300A   01B2   ST   1      SetupTechniqueDescription
300A   01B4   SQ   1      SetupDeviceSequence
300A   01B6   CS   1      SetupDeviceType
300A   01B8   SH   1      SetupDeviceLabel
300A   01BA   ST   1      SetupDeviceDescription
300A   01BC   DS   1      SetupDeviceParameter
300A   01D0   ST   1      SetupReferenceDescription
300A   01D2   DS   1      TableTopVerticalSetupDisplacement
300A   01D4   DS   1      TableTopLongitudinalSetupDisplacement
300A   01D6   DS   1      TableTopLateralSetupDisplacement
300A   0200   CS   1      BrachyTreatmentTechnique
300A   0202   CS   1      BrachyTreatmentType
300A   0206   SQ   1      TreatmentMachineSequence
300A   0210   SQ   1      SourceSequence
300A   0212   IS   1      SourceNumber
300A   0214   CS   1      SourceType
300A   0216   LO   1      SourceManufacturer
300A   0218   DS   1      ActiveSourceDiameter
300A   021A   DS   1      ActiveSourceLength
300A   0222   DS   1      SourceEncapsulationNominalThickness
300A   0224   DS   1      SourceEncapsulationNominalTransmission
300A   0226   LO   1      SourceIsotopeName
300A   0228   DS   1      SourceIsotopeHalfLife
300A   022A   DS   1      ReferenceAirKermaRate
300A   022C   DA   1      AirKermaRateReferenceDate
300A   022E   TM   1      AirKermaRateReferenceTime
300A   0230   SQ   1      ApplicationSetupSequence
300A   0232   CS   1      ApplicationSetupType
300A   0234   IS   1      ApplicationSetupNumber
300A   0236   LO   1      ApplicationSetupName
300A   0238   LO   1      ApplicationSetupManufacturer
300A   0240   IS   1      TemplateNumber
300A   0242   SH   1      TemplateType
300A   0244   LO   1      TemplateName
300A   0250   DS   1      TotalReferenceAirKerma
300A   0260   SQ   1      BrachyAccessoryDeviceSequence
300A   0262   IS   1      BrachyAccessoryDeviceNumber
300A   0263   SH   1      BrachyAccessoryDeviceID
300A   0264   CS   1      BrachyAccessoryDeviceType
300A   0266   LO   1      BrachyAccessoryDeviceName
300A   026A   DS   1      BrachyAccessoryDeviceNominalThickness
300A   026C   DS   1      BrachyAccessoryDeviceNominalTransmission
300A   0280   SQ   1      ChannelSequence
300A   0282   IS   1      ChannelNumber
300A   0284   DS   1      ChannelLength
300A   0286   DS   1      ChannelTotalTime
300A   0288   CS   1      SourceMovementType
300A   028A   IS   1      NumberOfPulses
300A   028C   DS   1      PulseRepetitionInterval
300A   0290   IS   1      SourceApplicatorNumber
300A   0291   SH   1      SourceApplicatorID
300A   0292   CS   1      SourceApplicatorType
300A   0294   LO   1      SourceApplicatorName
300A   0296   DS   1      SourceApplicatorLength
300A   0298   LO   1      SourceApplicatorManufacturer
300A   029C   DS   1      SourceApplicatorWallNominalThickness
300A   029E   DS   1      SourceApplicatorWallNominalTransmission
300A   02A0   DS   1      SourceApplicatorStepSize
300A   02A2   IS   1      TransferTubeNumber
300A   02A4   DS   1      TransferTubeLength
300A   02B0   SQ   1      ChannelShieldSequence
300A   02B2   IS   1      ChannelShieldNumber
300A   02B3   SH   1      ChannelShieldID
300A   02B4   LO   1      ChannelShieldName
300A   02B8   DS   1      ChannelShieldNominalThickness
300A   02BA   DS   1      ChannelShieldNominalTransmission
300A   02C8   DS   1      FinalCumulativeTimeWeight
300A   02D0   SQ   1      BrachyControlPointSequence
300A   02D2   DS   1      ControlPointRelativePosition
300A   02D4   DS   3      ControlPointDPosition
300A   02D6   DS   1      CumulativeTimeWeight
300C   0000   UL   1      RTRelationshipGroupLength
300C   0002   SQ   1      ReferencedRTPlanSequence
300C   0004   SQ   1      ReferencedBeamSequence
300C   0006   IS   1      ReferencedBeamNumber
300C   0007   IS   1      ReferencedReferenceImageNumber
300C   0008   DS   1      StartCumulativeMetersetWeight
300C   0009   DS   1      EndCumulativeMetersetWeight
300C   000A   SQ   1      ReferencedBrachyApplicationSetupSequence
300C   000C   IS   1      ReferencedBrachyApplicationSetupNumber
300C   000E   IS   1      ReferencedSourceNumber
300C   0020   SQ   1      ReferencedFractionGroupSequence
300C   0022   IS   1      ReferencedFractionGroupNumber
300C   0040   SQ   1      ReferencedVerificationImageSequence
300C   0042   SQ   1      ReferencedReferenceImageSequence
300C   0050   SQ   1      ReferencedDoseReferenceSequence
300C   0051   IS   1      ReferencedDoseReferenceNumber
300C   0055   SQ   1      BrachyReferencedDoseReferenceSequence
300C   0060   SQ   1      ReferencedStructureSetSequence
300C   006A   IS   1      ReferencedPatientSetupNumber
300C   0080   SQ   1      ReferencedDoseSequence
300C   00A0   IS   1      ReferencedToleranceTableNumber
300C   00B0   SQ   1      ReferencedBolusSequence
300C   00C0   IS   1      ReferencedWedgeNumber
300C   00D0   IS   1      ReferencedCompensatorNumber
300C   00E0   IS   1      ReferencedBlockNumber
300C   00F0   IS   1      ReferencedControlPointIndex
300E   0000   UL   1      RTApprovalGroupLength
300E   0002   CS   1      ApprovalStatus
300E   0004   DA   1      ReviewDate
300E   0005   TM   1      ReviewTime
300E   0008   PN   1      ReviewerName
4000   0000   UL   1      TextGroupLength
4000   0010   LT   1-n    TextArbitrary
4000   4000   LT   1-n    TextComments
4008   0000   UL   1      ResultsGroupLength
4008   0040   SH   1      ResultsID
4008   0042   LO   1      ResultsIDIssuer
4008   0050   SQ   1      ReferencedInterpretationSequence
4008   0100   DA   1      InterpretationRecordedDate
4008   0101   TM   1      InterpretationRecordedTime
4008   0102   PN   1      InterpretationRecorder
4008   0103   LO   1      ReferenceToRecordedSound
4008   0108   DA   1      InterpretationTranscriptionDate
4008   0109   TM   1      InterpretationTranscriptionTime
4008   010A   PN   1      InterpretationTranscriber
4008   010B   ST   1      InterpretationText
4008   010C   PN   1      InterpretationAuthor
4008   0111   SQ   1      InterpretationApproverSequence
4008   0112   DA   1      InterpretationApprovalDate
4008   0113   TM   1      InterpretationApprovalTime
4008   0114   PN   1      PhysicianApprovingInterpretation
4008   0115   LT   1      InterpretationDiagnosisDescription
4008   0117   SQ   1      DiagnosisCodeSequence
4008   0118   SQ   1      ResultsDistributionListSequence
4008   0119   PN   1      DistributionName
4008   011A   LO   1      DistributionAddress
4008   0200   SH   1      InterpretationID
4008   0202   LO   1      InterpretationIDIssuer
4008   0210   CS   1      InterpretationTypeID
4008   0212   CS   1      InterpretationStatusID
4008   0300   ST   1      Impressions
4008   4000   ST   1      ResultsComments
5000   0000   UL   1      CurveGroupLength
5000   0005   US   1      CurveDimensions
5000   0010   US   1      NumberOfPoints
5000   0020   CS   1      TypeOfData
5000   0022   LO   1      CurveDescription
5000   0030   SH   1-n    AxisUnits
5000   0040   SH   1-n    AxisLabels
5000   0103   US   1      DataValueRepresentation
5000   0104   US   1-n    MinimumCoordinateValue
5000   0105   US   1-n    MaximumCoordinateValue
5000   0106   SH   1-n    CurveRange
5000   0110   US   1      CurveDataDescriptor
5000   0112   US   1      CoordinateStartValue
5000   0114   US   1      CoordinateStepValue
5000   2000   US   1      AudioType
5000   2002   US   1      AudioSampleFormat
5000   2004   US   1      NumberOfChannels
5000   2006   UL   1      NumberOfSamples
5000   2008   UL   1      SampleRate
5000   200A   UL   1      TotalTime
5000   200C   OX   1      AudioSampleData
5000   200E   LT   1      AudioComments
5000   3000   OX   1      CurveData
5400   0100   SQ   1      WaveformSequence
5400   0110   OX   1      ChannelMinimumValue
5400   0112   OX   1      ChannelMaximumValue
5400   1004   US   1      WaveformBitsAllocated
5400   1006   CS   1      WaveformSampleInterpretation
5400   100A   OX   1      WaveformPaddingValue
5400   1010   OX   1      WaveformData
6000   0000   UL   1      OverlayGroupLength
6000   0010   US   1      OverlayRows
6000   0011   US   1      OverlayColumns
6000   0012   US   1      OverlayPlanes
6000   0015   IS   1      OverlayNumberOfFrames
6000   0040   CS   1      OverlayType
6000   0050   SS   2      OverlayOrigin
6000   0051   US   1      OverlayImageFrameOrigin
6000   0052   US   1      OverlayPlaneOrigin
6000   0060   CS   1      OverlayCompressionCode
6000   0061   SH   1      OverlayCompressionOriginator
6000   0062   SH   1      OverlayCompressionLabel
6000   0063   SH   1      OverlayCompressionDescription
6000   0066   AT   1-n    OverlayCompressionStepPointers
6000   0068   US   1      OverlayRepeatInterval
6000   0069   US   1      OverlayBitsGrouped
6000   0100   US   1      OverlayBitsAllocated
6000   0102   US   1      OverlayBitPosition
6000   0110   CS   1      OverlayFormat
6000   0200   US   1      OverlayLocation
6000   0800   CS   1-n    OverlayCodeLabel
6000   0802   US   1      OverlayNumberOfTables
6000   0803   AT   1-n    OverlayCodeTableLocation
6000   0804   US   1      OverlayBitsForCodeWord
6000   1100   US   1      OverlayDescriptorGray
6000   1101   US   1      OverlayDescriptorRed
6000   1102   US   1      OverlayDescriptorGreen
6000   1103   US   1      OverlayDescriptorBlue
6000   1200   US   1-n    OverlayGray
6000   1201   US   1-n    OverlayRed
6000   1202   US   1-n    OverlayGreen
6000   1203   US   1-n    OverlayBlue
6000   1301   IS   1      ROIArea
6000   1302   DS   1      ROIMean
6000   1303   DS   1      ROIStandardDeviation
6000   3000   OW   1      OverlayData
6000   4000   LT   1-n    OverlayComments
7F00   0000   UL   1      VariablePixelDataGroupLength
7F00   0010   OX   1      VariablePixelData
7F00   0011   AT   1      VariableNextDataGroup
7F00   0020   OW   1-n    VariableCoefficientsSDVN
7F00   0030   OW   1-n    VariableCoefficientsSDHN
7F00   0040   OW   1-n    VariableCoefficientsSDDN
7FE0   0000   UL   1      PixelDataGroupLength
7FE0   0010   OB   1      PixelData
7FE0   0020   OW   1-n    CoefficientsSDVN
7FE0   0030   OW   1-n    CoefficientsSDHN
7FE0   0040   OW   1-n    CoefficientsSDDN
FFFC   FFFC   OB   1      DataSetTrailingPadding
FFFE   E000   NONE 1      Item
FFFE   E00D   NONE 1      ItemDelimitationItem
FFFE   E0DD   NONE 1      SequenceDelimitationItem
END_DICOM_FIELDS


## others that we aren't using right now
#  0020   31xx   LT   1-n    SourceImageID
#  0020   31xx   CS   1-n    SourceImageID
#  0028   04x0   US   1      RowsForNthOrderCoefficients
#  0028   04x1   US   1      ColumnsForNthOrderCoefficients
#  0028   04x2   LO   1-n    CoefficientCoding
#  0028   04x3   AT   1-n    CoefficientCodingPointers
#  0028   08x0   LO   1-n    CodeLabel
#  0028   08x2   US   1      NumberOfTables
#  0028   08x3   AT   1-n    CodeTableLocation
#  0028   08x4   US   1      BitsForCodeWord
#  0028   08x8   AT   1-n    ImageDataLocation
#  1000   00x0   US   3      EscapeTriplet
#  1000   00x1   US   3      RunLengthTriplet
#  1000   00x2   US   1      HuffmanTableSize
#  1000   00x3   US   3      HuffmanTableTriplet
#  1000   00x4   US   1      ShiftTableSize
#  1000   00x5   US   3      ShiftTableTriplet
#  1010   xxxx   US   1-n    ZonalMap
#  50xx   0000   UL   1      CurveGroupLength
#  50xx   0005   US   1      CurveDimensions
#  50xx   0010   US   1      NumberOfPoints
#  50xx   0020   CS   1      TypeOfData
#  50xx   0022   LO   1      CurveDescription
#  50xx   0030   SH   1-n    AxisUnits
#  50xx   0040   SH   1-n    AxisLabels
#  50xx   0103   US   1      DataValueRepresentation
#  50xx   0104   US   1-n    MinimumCoordinateValue
#  50xx   0105   US   1-n    MaximumCoordinateValue
#  50xx   0106   SH   1-n    CurveRange
#  50xx   0110   US   1      CurveDataDescriptor
#  50xx   0112   US   1      CoordinateStartValue
#  50xx   0114   US   1      CoordinateStepValue
#  50xx   1001   CS   1      CurveActivationLayer
#  50xx   2000   US   1      AudioType
#  50xx   2002   US   1      AudioSampleFormat
#  50xx   2004   US   1      NumberOfChannels
#  50xx   2006   UL   1      NumberOfSamples
#  50xx   2008   UL   1      SampleRate
#  50xx   200a   UL   1      TotalTime
#  50xx   200c   OW   1      AudioSampleData
#  50xx   200e   LT   1      AudioComments
#  50xx   2500   LO   1      CurveLabel
#  50xx   2600   SQ   1      CurveReferencedOverlaySequence
#  50xx   2610   US   1      CurveReferencedOverlayGroup
#  50xx   3000   OW   1      CurveData
#  50xx   0000   UL   1      CurveGroupLength
#  50xx   0005   US   1      CurveDimensions
#  50xx   0010   US   1      NumberOfPoints
#  50xx   0020   CS   1      TypeOfData
#  50xx   0022   LO   1      CurveDescription
#  50xx   0030   SH   1-n    AxisUnits
#  50xx   0040   SH   1-n    AxisLabels
#  50xx   0103   US   1      DataValueRepresentation
#  50xx   0104   US   1-n    MinimumCoordinateValue
#  50xx   0105   US   1-n    MaximumCoordinateValue
#  50xx   0106   SH   1-n    CurveRange
#  50xx   0110   US   1-n    CurveDataDescriptor
#  50xx   0112   US   1      CoordinateStartValue
#  50xx   0114   US   1      CoordinateStepValue
#  50xx   1001   CS   1      CurveActivationLayer
#  50xx   2000   US   1      AudioType
#  50xx   2002   US   1      AudioSampleFormat
#  50xx   2004   US   1      NumberOfChannels
#  50xx   2006   UL   1      NumberOfSamples
#  50xx   2008   UL   1      SampleRate
#  50xx   200A   UL   1      TotalTime
#  50xx   200C   OW/OB 1      AudioSampleData
#  50xx   200E   LT   1      AudioComments
#  50xx   2500   LO   1      CurveLabel
#  50xx   2600   SQ   1      CurveReferencedOverlaySeq
#  50xx   2610   US   1      ReferencedOverlayGroup
#  50xx   3000   OW/OB 1      CurveData
#  60xx   0000   UL   1      OverlayGroupLength
#  60xx   0010   US   1      OverlayRows
#  60xx   0011   US   1      OverlayColumns
#  60xx   0012   US   1      OverlayPlanes
#  60xx   0015   IS   1      NumberOfFramesInOverlay
#  60xx   0022   LO   1      OverlayDescription
#  60xx   0040   CS   1      OverlayType
#  60xx   0045   CS   1      OverlaySubtype
#  60xx   0050   SS   2      OverlayOrigin
#  60xx   0051   US   1      ImageFrameOrigin
#  60xx   0052   US   1      PlaneOrigin
#  60xx   0060   LT   1      OverlayCompressionCode
#  60xx   0061   SH   1      OverlayCompressionOriginator
#  60xx   0062   SH   1      OverlayCompressionLabel
#  60xx   0063   SH   1      OverlayCompressionDescription
#  60xx   0066   AT   1-n    OverlayCompressionStepPointers
#  60xx   0068   US   1      OverlayRepeatInterval
#  60xx   0069   US   1      OverlayBitsGrouped
#  60xx   0100   US   1      OverlayBitsAllocated
#  60xx   0102   US   1      OverlayBitPosition
#  60xx   0110   LT   1      OverlayFormat
#  60xx   0200   US   1      OverlayLocation
#  60xx   0800   LO   1-n    OverlayCodeLabel
#  60xx   0802   US   1      OverlayNumberOfTables
#  60xx   0803   AT   1-n    OverlayCodeTableLocation
#  60xx   0804   US   1      OverlayBitsForCodeWord
#  60xx   1001   CS   1      OverlayActivationLayer
#  60xx   1100   US   1      OverlayDescriptorGray
#  60xx   1101   US   1      OverlayDescriptorRed
#  60xx   1102   US   1      OverlayDescriptorGreen
#  60xx   1103   US   1      OverlayDescriptorBlue
#  60xx   1200   US   1-n    OverlayGray
#  60xx   1201   US   1-n    OverlayRed
#  60xx   1202   US   1-n    OverlayGreen
#  60xx   1203   US   1-n    OverlayBlue
#  60xx   1301   IS   1      ROIArea
#  60xx   1302   DS   1      ROIMean
#  60xx   1303   DS   1      ROIStandardDeviation
#  60xx   1500   LO   1      OverlayLabel
#  60xx   3000   OW   1      OverlayData
#  60xx   4000   LT   1-n    OverlayComments
#  60xx   0000   UL   1      OverlayGroupLength
#  60xx   0010   US   1      OverlayRows
#  60xx   0011   US   1      OverlayColumns
#  60xx   0012   US   1      OverlayPlanes
#  60xx   0015   IS   1      NumberOfFramesInOverlay
#  60xx   0022   LO   1      OverlayDescription
#  60xx   0040   CS   1      OverlayType
#  60xx   0045   LO   1      OverlaySubtype
#  60xx   0050   SS   2      OverlayOrigin
#  60xx   0051   US   1      ImageFrameOrigin
#  60xx   0052   US   1      OverlayPlaneOrigin
#  60xx   0060   CS   1      OverlayCompressCode
#  60xx   0061   SH   1      OverlayCompressOriginator
#  60xx   0062   SH   1      OverlayCompressLabel
#  60xx   0063   SH   1      OverlayCompressDescription
#  60xx   0066   AT   1-n    OverlayCompressStepPointers
#  60xx   0068   US   1      OverlayRepeatInterval
#  60xx   0069   US   1      OverlayBitsGrouped
#  60xx   0100   US   1      OverlayBitsAllocated
#  60xx   0102   US   1      OverlayBitPosition
#  60xx   0110   CS   1      OverlayFormat
#  60xx   0200   US   1      OverlayLocation
#  60xx   0800   CS   1-n    OverlayCodeLabel
#  60xx   0802   US   1      OverlayNumberOfTables
#  60xx   0803   AT   1-n    OverlayCodeTableLocation
#  60xx   0804   US   1      OverlayBitsForCodeWord
#  60xx   1001   CS   1      OverlayActivationLayer
#  60xx   1100   US   1      OverlayDescriptorGray
#  60xx   1101   US   1      OverlayDescriptorRed
#  60xx   1102   US   1      OverlayDescriptorGreen
#  60xx   1103   US   1      OverlayDescriptorBlue
#  60xx   1200   US   1-n    OverlayGray
#  60xx   1201   US   1-n    OverlayRed
#  60xx   1202   US   1-n    OverlayGreen
#  60xx   1203   US   1-n    OverlayBlue
#  60xx   1301   IS   1      ROIArea
#  60xx   1302   DS   1      ROIMean
#  60xx   1303   DS   1      ROIStandardDeviation
#  60xx   1500   LO   1      OverlayLabel
#  60xx   3000   OX   1      OverlayData
#  60xx   4000   LT   1-n    OverlayComments
#  7Fxx   0000   UL   1      VariablePixelDataGroupLength
#  7Fxx   0010   OW   1      VariablePixelData
#  7Fxx   0011   US   1      VariableNextDataGroup
#  7Fxx   0020   OW   1-n    VariableCoefficientsSDVN
#  7Fxx   0030   OW   1-n    VariableCoefficientsSDHN
#  7Fxx   0040   OW   1-n    VariableCoefficientsSDDN

1;
 
__END__

=head1 NAME

DICOM::Fields - Definition of DICOM fields

=head1 SYNOPSIS

  use DICOM::Fields qw/@dicom_fields/;
  
  my %dicom_dict;
  
  # Initialise dictionary.
  my ($line, $group, $element, $vr, $size, $name);
  
  
  foreach $line (@dicom_fields){
     ($group, $element, $vr, $size, $name) = split(/\s+/, $line);
     $dicom_dict{hex($group)}{hex($element)} = [($vr, $size, $name)];
     }

=head1 DESCRIPTION

This module is little more than a great big list of all the DICOM fields
that you would typically want to use with imaging data.

=head1 SEE ALSO

DICOM DICOM::VR DICOM::Element

=head1 AUTHOR

Andrew Crabb E<lt>ahc@jhu.eduE<gt>,
Jonathan Harlap E<lt>jharlap@bic.mni.mcgill.caE<gt>,
Andrew Janke E<lt>a.janke@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2002 by Andrew Crabb
Some parts are Copyright (C) 2003 by Jonathan Harlap
And some Copyright (C) 2009 by Andrew Janke

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.6.0 or,
at your option, any later version of Perl 5 you may have available.

=cut
