# Revision notes

- Added `AuthenticationSetHospitalId()`.
- Added A1+ to `BmsBloodGroup_t`.
- Fixed missing includes in blood-group/date validation.
- Added reusable next-ID generation based on the maximum stored ID.
- Revised Hospital Staff menu and hospital-link workflow.
- Revised Create Blood Request screen to display generated Request ID and linked Hospital automatically.
- Removed manual creation-ID prompts for donor, hospital, inventory, donation, request, and emergency alert records.
- Verified compilation using GCC C11 with strict warnings.
