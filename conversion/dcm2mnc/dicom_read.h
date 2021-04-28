extern void get_dicom_image_data(Acr_Group group_list, Image_Data *image);
extern void parse_dicom_groups(Acr_Group group_list, Data_Object_Info *di_ptr);
extern void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info, const char *file_name);

#define DICOM_POSITION_LOCAL 2
#define DICOM_POSITION_GLOBAL 1
#define DICOM_POSITION_NONE 0

extern int dicom_read_position(Acr_Group group_list, int n, double position[3]);
extern int dicom_read_orientation(Acr_Group group_list, double orientation[6]);
extern int dicom_read_pixel_size(Acr_Group group_list, double pixel_size[2]);

extern double get_slice_separation( Acr_Group group_list);
extern void calculate_dircos(double RowColVec[6], double dircos[VOL_NDIMS][WORLD_NDIMS], int do_coordinate_conversion);
extern double convert_seconds_to_time(double seconds);
extern double convert_time_to_seconds(double dicomtime);

extern void convert_dicom_coordinate(double coord[]);
extern int is_numaris3(Acr_Group group_list);

Acr_Element acr_recursive_search(Acr_Element element, int *nskip,
                                 Acr_Element_Id search_id);
Acr_Element acr_recurse_for_element(Acr_Group group_list,
                                    int nskip,
                                    Acr_Element_Id seq_id,
                                    Acr_Element_Id srch_id);
