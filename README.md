# tablet-counter

A simple program used to count many items.

![original image](https://github.com/quinson/tablet-counter/blob/master/img/original.gif)

In the early stages, contours will be formed from a bilinear image after identifying the primary color of the object.

![outline of objects](https://github.com/quinson/tablet-counter/blob/master/img/drawing.gif)

A rough estimate can be found be the sum of the arc length of the countours, divided by the median length. This approximation works as long as the majority of the contours surround a single object.

The project will reach a final conclusion when each tablet can be identified individually, and a marker placed on each tablet to ensure accurate readings in accordance to FDA regulation.
