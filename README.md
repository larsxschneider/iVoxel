# iVoxel

A voxel rendering engine for iOS and Mac OS X. The implementation leverages out-of-core rendering techniques, custom memory allocation, tightly packed data structures and GPU computation to enable real-time rendering performance. Take a look at [my thesis](https://github.com/downloads/larsxschneider/MasterThesis/schneider-lars_msc_thesis.pdf) for more information

In "Assets/PointClouds" you find a version of the [Stanford models](http://graphics.stanford.edu/data/3Dscanrep/) in the required data format for this project.

## Videos

[Standford Lucy and Manuscript on iPhone 3GS and iPad](http://www.youtube.com/watch?v=XPciH3Ynci0&t=3m28s)

[Point cloud on iPhone 4](http://www.youtube.com/watch?v=vkzlpXr7au4)

## Deployment

The iOS app runs only on the device. Simulator is not supported.

To run the Mac OS X app please adjust the path to the point data ("AppCore.cpp" line 50).

## Contact

Lars Schneider <larsxschneider+ivoxel@gmail.com>


## License

iVoxel is available under the BSD license. See the LICENSE file for more info.