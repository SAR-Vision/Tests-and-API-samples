For run tests add one of te following row:
"$(KAYA_VISION_POINT_BIN_PATH_64)\VPGenTL_vc141.cti" /nodatastream /r 0-120 /o "$(OutDir)\report"    // For testing VP2 GenTL producer
"$(KAYA_VISION_POINT_BIN_PATH)\KYFGLibGenTL_vc141.cti" /nodatastream /r 0-100 /o "$(OutDir)\report"  // For testing VP1 GenTL producer

Copy row to:
GenTLValidCon14->Properties->Debugging->Comand Arguments

