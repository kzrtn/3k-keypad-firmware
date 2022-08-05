# Keep the BOOTSEL button pressed while connecting the USB of the pico to the PC to enter the bootloader

# To give this the user to run this shell script run the following command:
# chmod +x Flash_code.sh

# Define red and green text
RED='\033[0;31m'
GREEN='\033[0;32m'
NOCOLOR='\033[0m'

# Copy the ... to the pi pico
cp build/src/3k_keypad.uf2 /media/$USER/RPI-RP2/

# Check if the copy was successful and report to the user
if [ $? -eq 0 ]; then
   echo "${GREEN}[SUCCESS]${NOCOLOR} File was uploaded to the pico without an issue."
else
   echo "${RED}[FAIL]${NOCOLOR} Something went wrong while trying to upload the file to the pico." 
   echo "       Don't forget to hold the BOOTSEL button while connecting the USB of the "
   echo "       pico to your PC."
fi

# Wait for a short while so the user can read potential errors
echo ""
echo "Auto close in 10 seconds..."
sleep 10


# After the file is copied the pico dismounts itself
