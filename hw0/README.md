# Assignment 0
Name: Thai Khong

All necessary header files are included in the `/include` folder. 
Each header file has a block of comment explaining its purpose.

## Part 1
The code is written in `parse_object.cpp`.

To run the program, run `make` to create the `parse_object` executable.
Then, run `./parse_object <filename1.obj> <filename2.obj> ...` to parse
one or multiple object files and print the resulting data to the output.

## Part 2
The code is written in `parse_transform.cpp`.

To run the program, run `make` to create the `parse_transform` executable.
Then, run `./parse_transform <transform_data.txt>` to parse the transformation text 
file and print the resulting matrix of the inverse of product of all transformations
to the output.

There might be some instances when the matrix is printed out where
a `0` value shows up as an extremely small value like `10e-18`. This is 
probably due to floating point errors, and these small values are 
intended to be `0`.

## Part 3
The code is written in `transform.cpp`.

To run the program, run `make` to create the `transform` executable.
Then, run `./transform <test_file.txt>` to parse the data file and print 
the resulting objects, its copy number, and its transformed vertices
to the output.

## Part 4
The code is written in `ppm_gen.cpp`.

To run the program, run `make` to create the `ppm_test` executable.
Then, run `./ppm_test xres yres`, where `xres` and `yres` defines a
`xres` x `yres` pixel grid, to print to the output the resulting `.ppm` file
that depicts a yellow circle contained in a dark red rectangular background.
To convert the output to a `.png` image, run 
`./ppm_test xres yres | convert - my_image_name.png` instead.

It is recommended to have `xres` and `yres` be larger than 50 both in order
for the circle to work properly. Any smaller values will result in a slightly
pointy circle instead.