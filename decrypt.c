/************************************
 Decryption algorithm, generated by Cruz 
 Vargas to solve HW1 from Jonathan Katz's 
 Introduction to Classical Cryptography
 ************************************/

#include <stdio.h>
#include <stdlib.h>     /* strtol */
#include <math.h>
#include <string.h>
#define MAX_KEY_LENGTH 24
#define LOGGING 0

int main(){
    unsigned char ch1, ch2;
    int byte;
    FILE *fpIn, *fpOut;
    int i;
    int distribution[MAX_KEY_LENGTH][255];//array of key lengths & distributions for each key length
    float probability[MAX_KEY_LENGTH];
    int max[MAX_KEY_LENGTH], min[MAX_KEY_LENGTH]; //max and min of ASCII as integers
    float files_char_count[MAX_KEY_LENGTH];
    int key_length;
    
    //unclear why the fuck this is necessary - distribution array not be initialized to 0
    memset(distribution, 0, sizeof(distribution[0][0]) * MAX_KEY_LENGTH * 255);
    memset(max, 0, sizeof(max[0]) * MAX_KEY_LENGTH);
    memset(min, 0, sizeof(max[0]) * MAX_KEY_LENGTH);
    memset(probability, 0, sizeof(probability[0]) * MAX_KEY_LENGTH);
    
    fpIn = fopen("ctext.txt", "r");
    fpOut = fopen("ptext_decoded.txt", "w");
    
    //log the distribution of character occurence
    for (int n = 1; n < MAX_KEY_LENGTH; n++) {
        
        i=1;
        files_char_count[n] = 0;

        while (fscanf(fpIn, "%02X", &byte) != EOF) {
            if (i % n == 0) {
                distribution[n][byte] ++; //track occurence in corresponding int slot of array
                // if (min[n] == 0) {min[n] = byte;}
                // if (max[n] < byte) {max[n] = byte;}
                files_char_count[n]++; //total number of chars considered
            }
            i++;

        }
        rewind(fpIn); //be kind, rewind
    }
    
    //get the key length distributions
    float max_probability;
    float a;
    for (int k = 1; k < MAX_KEY_LENGTH ; k++) {
        for (int l = 0; l < 255; l++) {
            //((number of times shown) / (files_char_count))^2
            a = powf((((float) distribution[k][l]) / (files_char_count[k]) ),2);
            //add probability squared to that key length's sum
            probability[k] = (float)probability[k] + (float)a;
            //record max probability as key length
            if (probability[k] > max_probability) {
                max_probability = probability[k];
                key_length = k;//k;
            }
        }
        //log out the probabilities
        fprintf(stdout, "length %02d gives %1.3f probability with %02.0f chars\n", k, probability[k], files_char_count[k]);
    }
    fprintf(stdout,"key length is likely %d\n\n", key_length);
    
    //now that we know the length, find the actual key
    char key[key_length];//an array to store the guess at the key
    memset(key, 0, sizeof(key[0]) * key_length);

    //start a ith byte,
    //collect every ith byte,
    // char analysis[key_length][255];
    // float shifted_char_count[key_length];
    // float key_probability[key_length];
    int eligible_chars = 127-32;
    float xored_permutations[key_length + 1][eligible_chars + 32][255][1];
    // float collapsed_probabilities[key_length + 1][eligible_chars + 32][1];
    // float number_of_chars[key_length];

    // memset(xored_permutations, 0, sizeof(xored_permutations[0][0][0][0]) * key_length * 255 * (eligible_chars + 32)); 
    // memset(key_probability, 0, sizeof(key_probability[0]) * key_length);
    // memset(analysis, 0, sizeof(analysis[0][0]) * key_length * (255));
    // memset(max, 0, sizeof(max[0]) * MAX_KEY_LENGTH);
    // memset(min, 0, sizeof(max[0]) * MAX_KEY_LENGTH);



    // for (int key_shift = 0; key_shift < key_length +1; key_shift++) { //each shift, through end of key length
    //     //start at i    
    int current_char_count = 0;
    int minor_char_count = 0;
    int xored_chars[key_length][ (int) files_char_count[key_length]][255]; //need int because rollover if not?
    memset(xored_chars, 0, sizeof(xored_chars[0][0][0]) * key_length * ((int) files_char_count[key_length]) * 255); // anything is POSSIBLEEEE
    int iteration = 0;

        while (fscanf(fpIn, "%02X", &byte) != EOF) {
            // if (current_char_count % key_length == 0){ minor_char_count = 0; }//make this a dimension, where ccc%kl is a dimension of the xored chars
            minor_char_count = current_char_count % key_length;
                for (int xor = 0; xor < 255;  xor++)
                {
                    //try decrypting this character with every value, 0 to 255
                    xored_chars[minor_char_count][iteration][xor] = byte ^ (xor);

                    // if (xored_chars[minor_char_count][xor] > 32 && xored_chars[minor_char_count][xor] < 172)
                    //    {
                    // printf("%d xored with %d = %d\n", byte, xor, xored_chars[minor_char_count][xor]);
                    // printf("%c xored with %c = %c\n", byte, xor, xored_chars[minor_char_count][xor]);
                    // }   

                }
            if (minor_char_count == 13){iteration++;}

                // minor_char_count++;  
            // }
            current_char_count++;
        }

//// testing print ////
for (int keyspot = 0; keyspot < key_length; keyspot++)
{
for (int xoredwithchar = 0; xoredwithchar < 255; ++xoredwithchar)
{
printf("spot %d xwith %d:", keyspot, xoredwithchar);
for (int filechunk = 0; filechunk < ((int) files_char_count[key_length]); filechunk++)
{
    printf("%x=%c ", xored_chars[keyspot][filechunk][xoredwithchar], xored_chars[keyspot][filechunk][xoredwithchar]);
}
printf("\n");
}
}


char possible_keys[key_length][255];
memset(possible_keys, 1, sizeof(possible_keys[0][0]) * key_length * 255); // anything is POSSIBLEEEE
int numberoffailures[key_length][255];

for (int i = 0; i < key_length; i++)
{

for (int xoredwith = 0; xoredwith < 255; xoredwith++)
{
        //validate the xors that are valid
        for (int occurence = 0; occurence < ((int) files_char_count[key_length]); occurence++)
        {
            //does that xor value result in 32 < all values < 127
            if ((xored_chars[i][occurence][xoredwith] < 32) || (xored_chars[i][occurence][xoredwith] > 127) || 
               ((xored_chars[i][occurence][xoredwith] > 47) && (xored_chars[i][occurence][xoredwith] < 58)) ) 
            {
                possible_keys[i][xoredwith] = 0; //TODO: hardcoded
                numberoffailures[i][xoredwith]++;
                // printf("xored_chars[%d][%d][%d] = %d\n", i, occurence, xoredwith, xored_chars[i][occurence][xoredwith]);
            }
        }
}

}

for (int j = 0; j < key_length; ++j)
{
for (int i = 0; i < 255 ; ++i)
{
    printf("%c/%d failed %d times for entry %d\n", i, i, numberoffailures[j][i], j);
}
}

    // float max_probability_key;
    // int potential_key_char;
    // int decoded_char;
    // float b;

    printf("the key is:");
    for (int h = 0; h < key_length +1; h++) {
        fprintf(stdout,"%c",key[h]);
    }
    printf("\n");


    for (int q = 0; q < key_length; q++)
    {
        printf("key[%d]:", q);
        printf("key possibles: ");
        for (int r = 0; r < 255; ++r)
        {
            if(possible_keys[q][r] == 1){ //TODO: hardcoded
                //if the array of possible key values is 1, print that array index as a char with ,
                printf("%d,", r);
                // printf("found one");
            }
        }
        printf("\n");
    }

    
    //now that we know the key, print the decrypted text out to a file
    int lol;
        while (fscanf(fpIn, "%02X", &byte) != EOF) {
            fprintf(stdout, "%c", byte ^ key[lol % key_length]);
            lol++;
        }
        printf("\n");
    
    
    
    fclose(fpIn);
    fclose(fpOut);  
    return 0;
}
