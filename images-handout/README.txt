15-122 Principles of Imperative Computation
Images

==========================================================

Files you will modify:
   imageutil.c0       - Skeleton image helper functions

Files you won't modify:
   remove-blue.c0      - You can modify this to respect the pixel/imageutil
                        interfaces if you'd like
   remove-blue-main.c0 - Runs remove_blue()
   remove-blue-test.c0 - Some unit tests for remove_blue()
   rotate-main.c0     - Runs rotate()
   maskblur-main.c0   - Runs apply_mask() to do bluring
   maskedge-main.c0   - Runs apply_mask() to do edge detection

Files that don't exist yet:
   pixel.c0           - Copy this over from Programming 1 (Pixels)
   rotate.c0          - Task 3
   mask.c0            - Task 4
   images-test.c0     - test your manipulations (optional)

Data:
   blur-slightly-mask.txt - 3x3 blur mask
   blur-mask.txt - 5x5 blur mask
   blur-more-mask.txt - 5x5 blur mask
   sobelX.txt - 3x3 Sobel filter
   sobelY.txt - 3x3 Sobel filter
   images/g5.png
   images/carnegie.png
   images/scs.png
   images/cmu.png
   images/tinytestpattern.png - 3x2 image, may be good for testing
   images/g5-remove-blue.png - result of remove_blue()
   images/g5-remove-blue-bug.png - result of a buggy remove_blue()
   images/carnegie-rotate.png - result of rotate()
   images/cmu-gaussian.png - result of maskblur with blur-slightly-mask.txt
   images/cmu-edge.png - result of maskblur with mask blur-mask.txt

==========================================================

Using the imagediff utility: you have been given the result of running
remove_blue on both a correct and a buggy implementation of
g5.png. Running the following command shows that the two images differ
by 600 pixels.

   % imagediff -i images/g5-remove-blue.png -j images/g5-remove-blue-bug.png
   Loaded image images/g5-remove-blue.png. Dimensions are 800 by 600.
   Loaded image images/g5-remove-blue-bug.png. Dimensions are 800 by 600.
   Number of pixels with different colors: 600 out of 480000.
   0

If you want to visually see where these 600 different pixels actually
are, you can get imagediff to print out a new image that highlights
the places where differences occured:

   % imagediff -i images/g5-remove-blue.png -j images/g5-remove-blue-bug.png -o diff.png
   % eog diff.png &

In addition to writing test cases, this can be a useful way of
debugging your code.

==========================================================

Compiling remove-blue and test cases:
   % cc0 -d -w -o remove-blue-test pixel.c0 imageutil.c0 remove-blue.c0 remove-blue-test.c0
   % ./remove-blue-test

   % cc0 -d -w -o remove-blue pixel.c0 imageutil.c0 remove-blue.c0 remove-blue-main.c0
   % ./remove-blue -i images/g5.png -o images/g5-my-output.png
       (This creates the output file images/g5-my-output.png. If you
        left off the "-o images/g5-my-output.png" part, the output
        file would be called images/g5_remove-blue.png.)

Compiling your unit tests. You can omit rotate.c0 or mask.c0 if you haven't written test cases for rotate() or mask(), respectively:
   % cc0 -d -w -o images-test pixel.c0 imageutil.c0 rotate.c0 mask.c0 images-test.c0
   % ./images-test

Compiling rotate:
   % cc0 -d -w -o rotate pixel.c0 imageutil.c0 rotate.c0 rotate-main.c0
   % ./rotate -i images/carnegie.png -o images/carnegie-my-output.png
       (This creates the output file images/carnegie-my-output.png. If
        you left off the "-o images/carnegie-my-output.png" part, the
        output file would be called images/carnegie_rotate.png.)
   % imagediff -i images/carnegie-rotate.png -j images/carnegie-my-output.png

Compiling mask:
   % cc0 -d -w -o maskblur pixel.c0 imageutil.c0 mask.c0 maskblur-main.c0
   % ./maskblur -i images/cmu.png -o images/cmu-my-maskblur.png -m blur-more-mask.txt
       (This creates the output file images/cmu-my-maskblur.png. If you
        left off the "-o images/cmu-my-maskblur.png" part, the output
        file would be called images/cmu_maskblur.png.)
   % imagediff -i images/cmu-gaussian.png -j images/cmu-my-maskblur.png

   % cc0 -d -w -o maskedge pixel.c0 imageutil.c0 mask.c0 maskedge-main.c0
   % ./maskedge -i images/cmu.png -o images/cmu-my-maskedge.png
       (This creates the output file images/cmu-my-maskedge.png. If you
        left off the "-o images/cmu-my-maskedge.png" part, the output
        file would be called images/cmu_maskedge.png.)
   % imagediff -i images/cmu-edge.png -j images/cmu-my-maskedge.png

Compiling manipulate:
   % cc0 -d -w -o manipulate pixel.c0 imageutil.c0 rotate.c0 mask.c0 manipulate.c0 manipulate-main.c0
   % ./manipulate -i images/g5.png -o images/g5-my-manip.png

==========================================================

Submitting with Andrew handin script:
   % handin images imageutil.c0 rotate.c0 mask.c0 images-test.c0 manipulate.c0

Creating a tarball to submit with autolab.cs.cmu.edu web interface:
   % tar -czvf handin.tgz imageutil.c0 rotate.c0 mask.c0 images-test.c0 manipulate.c0
