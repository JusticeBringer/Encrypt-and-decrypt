# Encrypt-and-decrypt

Workflow:

Initial image is stored in internal memory and the secret keys are read from txt file\
There are generated random numbers, then a permutation and pixels are permuted\
The given array elements are xor-ed with a certain formula\
Image obtained is saved in external memory\
Semi-cripted image is stored in internal memory and secret keys are read from txt file\
It is generated an array of random elements, then a permutation and it is made an inverse permutation of pixels\
It is applied xor after a certain formula\
Cripted image is now saved in external memory\
Chi squared test is calculated and results are shown to the user.

### Examples

Input: 
![Peppers image](https://github.com/JusticeBringer/Encrypt-and-decrypt/blob/master/peppers.bmp)

Output:
1) Encrypted image
2) Decryption of the encrypted image
3) Results of how good the encrypted image is (in the window the code is running)

![Encrypted peppers image](https://github.com/JusticeBringer/Encrypt-and-decrypt/blob/master/cr_peppers.bmp)

Input:
![Person image](https://github.com/JusticeBringer/Encrypt-and-decrypt/blob/master/photo-github.bmp)

Output:
![Encrypted person image](https://github.com/JusticeBringer/Encrypt-and-decrypt/blob/master/cr_git.bmp)

### BMP converter

You need .bmp images for running this project. A tool that I used and seems to work is the following:

https://www.onlineconverter.com/jpg-to-bmp

### How to use the project

1) Convert the image you want to encrypt to bmp using the above link
2) Create a folder where you want your images to be stored
3) Copy the .bmp image to this folder
4) Create a .txt file named "secret_key" used for encryption. Set the content of this file to "123456789 987654321" or 2 numbers that you like
5) Change the main code lines from 570 to 581 to refer to your image location
6) After running the source code your images will be stored in your folder location
