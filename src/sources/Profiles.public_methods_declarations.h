OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       oyObject_s          object);
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         std_profile_class,
                                       int               * current,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyProfiles_DeviceRank ( oyProfiles_s    * list,
                                         oyConfig_s      * device,
                                         int32_t         * rank_list );
