# Disparity Map
Given a left and right image from a stereoscopic pair this produces a perceptual disparity map where each pixel in the map represents the horizontal disparity between matching features in the left and right image.

The method to produce the perceptual disparity map follows the algorithm described in:

"Towards compound stereo-video quality metric: a specific encoder-based framework"
Boev, A. Gotchev, A. giazarian, K. Aksay, A. Akar, G.B.

A source window in the left image is compared with a horizontal target region in the right image. 
Each comparison made in the target region is stored. The horizontal position of the highest matching target block is used as the perceived disparity.

To match the regions the SSIM (Structural Similarity) algorithm is used.

The implementation actually returns two disparity maps. The first returns the disparity mapped between 0-255. This is useful for viewing. The second disparity map returns the raw disparity found.

Note that it's not particularly fast, and the time it takes will vary upon the size of target region and the window size.

## Requirements
OpenCV

## Running
arg0 = path to left image
arg1 = path to right image
arg2 = size of target region to use for matching in the right image
arg3 = size of window for matching, i.e. the source window in the left image
arg4 = output file path

