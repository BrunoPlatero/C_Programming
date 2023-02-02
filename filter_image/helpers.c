#include "helpers.h"
#include "math.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // Iterate over all pixels
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate the average value
            float rgtbGrey = round((image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0);

            // Assign the average value to every pixel
            image[i][j].rgbtBlue = rgtbGrey;
            image[i][j].rgbtGreen = rgtbGrey;
            image[i][j].rgbtRed = rgtbGrey;
        }
    }
    return;
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate each new colour value and round it up
            float sepiaRed = round(0.393 * image[i][j].rgbtRed + 0.769 * image[i][j].rgbtGreen + 0.189 * image[i][j].rgbtBlue);
            float sepiaGreen = round(0.349 * image[i][j].rgbtRed + 0.686 * image[i][j].rgbtGreen + 0.168 * image[i][j].rgbtBlue);
            float sepiaBlue = round(0.272 * image[i][j].rgbtRed + 0.534 * image[i][j].rgbtGreen + 0.131 * image[i][j].rgbtBlue);

            // Ensure the result is between 0 and 255
            if (sepiaRed > 255)
            {
                sepiaRed = 255;
            }
            if (sepiaGreen > 255)
            {
                sepiaGreen = 255;
            }
            if (sepiaRed > 255)
            {
                sepiaGreen = 255;
            }

            // Assign sepia value
            image[i][j].rgbtRed = sepiaRed;
            image[i][j].rgbtGreen = sepiaGreen;
            image[i][j].rgbtBlue = sepiaBlue;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    int temp[3];
    for (int i = 0; i < height; i++)
    {
        // Iterate until reaching half of the width
        for (int j = 0; j < width / 2; j++)
        {
            // Declare temp variables to perform selection sort
            // Store in a temp variable the value of each pixel's data on each iteration

            temp[0] = image[i][j].rgbtRed;
            temp[1] = image[i][j].rgbtGreen;
            temp[2] = image[i][j].rgbtBlue;

            // Reflect image horizontally by:
            // 1st iteration last pixel becomes the first
            // 2nd iteration second last pixel becomes the second
            // And so on and so forth
            image[i][j].rgbtRed = image[i][width - j - 1].rgbtRed;
            image[i][j].rgbtGreen = image[i][width - j - 1].rgbtGreen;
            image[i][j].rgbtBlue = image[i][width - j - 1].rgbtBlue;

            // 1st iteration first pixel (we'd previously stored in temp) becomes the last
            // And so on
            image[i][width - j - 1].rgbtRed = temp[0];
            image[i][width - j - 1].rgbtGreen = temp[1];
            image[i][width - j - 1].rgbtBlue = temp[2];
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a copy of the image to work with
    RGBTRIPLE copy[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Declare variables to sum amount of colour in neighbour pixels
            int total_red = 0;
            int total_green = 0;
            int total_blue = 0;

            // Declare a counter to divide total_colour by 4, 6 or 9
            float counter = 0.0;

            // Start iterating one row and one column before the pixel on current interation until one row/column after
            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    // Current pixel to check is the one in the previous, same, or next row/column
                    int current_x = i + k;
                    int current_y = j + l;

                    // If current pixel is less than x or y, its location is out of the array (before first/last column)
                    // If current pixel is more than height or width, its location is out of the array (after last row/column)
                    // If so, skip
                    if (current_x < 0 || current_y < 0 || current_x > (height - 1) || current_y > (width - 1))
                    {
                        continue;
                    }

                    // Calculate the total amount of red, green, and blue of neighbour pixels
                    total_red += image[current_x][current_y].rgbtRed;
                    total_green += image[current_x][current_y].rgbtGreen;
                    total_blue += image[current_x][current_y].rgbtBlue;

                    // Increase the counter every time a neighbor pixel is checked
                    counter++;
                }

                // Calculate the average amount of colour and round it to int
                copy[i][j].rgbtRed = round(total_red / counter);
                copy[i][j].rgbtGreen = round(total_green / counter);
                copy[i][j].rgbtBlue = round(total_blue / counter);
            }
        }
    }

    // Copy the results from the copied image into the original image
    for (int i = 0; i < height; i ++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = copy[i][j].rgbtRed;
            image[i][j].rgbtGreen = copy[i][j].rgbtGreen;
            image[i][j].rgbtBlue = copy[i][j].rgbtBlue;
        }
    }
    return;
}
