# Procedural textures
### Infinite quality for 30 bytes

## Abstract

Since the late 90s the rise of computational power and storage capacity has led
to very much resource abuse. One of them is the huge waste of space for textures
used in CGI and video games. Let's face it: if you are looking for a decent
texture quality, you will need really big files. That's why video games textures
have not improved that much over the last decade.

The idea behind procedural textures is to describe the picture with mathematical
functions instead of describing every pixel individually. Hence their extremely
small size for a virtually infitine resolution. The big downside is the lack of
artistic freedom in texture design. It is not suitable for every purpose.


This small program is a simple implementation of the Perlin algorithm:

* Start from a random grayscale layer.

* Build from this layer a first octave, which is a new layer with frequency f,
  i.e. reuse only the points on a grid where with f nodes in width. The other
  pixels should be undefined.

* For each remaining pixel make a bi-interpolation of the 4 surrounding
  nodes. Cubic splines are great at interpolating.

* Continue with building the next octaves while increasing the frequency. A
  typical increase would  f *= original_f.

* Now the octaves are ready, they should be associated with a persistence, which
  can be seen as a transparency factor in [0,1]. Sum all the octaves to get the
  final result. The sum of the persistences should not exceed 1.

* Optional: apply filters. Interresting filters are:

  - Smoothing: for every pixel change its value to the mean of the n*n
    surrounding square.

  - Colors: until now we have worked in grayscale only. We can provide three
    color thresholds. All values between 0 and threshold1 will have color1. All
    values between threshold1 and threshold2 will have the mean value of color1
    and color2 weighted by the distance to the threshold. And so forth. See the
    source file for examples.

  - Cosine interpolation: this will generate sort of cells. If we mirror the
    threshold (see source code) it will result in a liquid/wave-looking texture.


--------------------------------------------------------------------------------

## Building

There is an embedded makefile. The only dependency is SDL. To build the program
simply run:

	$ make

This will build two standalone, independant executables:

* 'creator' for text-to-binary texture creation (see below).
* 'procedural-textures' to generate the graphic files from the binary data.

There is no `make install` since this program is a technology demo.

## Usage

First you need to create some textures. Textures are binary file (conventionnaly
ptx), so they may not be easy to create. I provided a small editor, the
'creator'. All you need is to type

	$ creator text-file binary-file

where text-file is a valid textual description. To get a clearer view over the
texture description file format, use an example:

	$ procedural-textures wood.ptx

## Links

* [Wikipedia: Texture](http://en.wikipedia.org/wiki/Procedural_texture)
* [Wikipedia: Texture](http://en.wikipedia.org/wiki/Texture_synthesis)
* [Wikipedia: Texture](http://en.wikipedia.org/wiki/Perlin_noise)
* [Ken Perlin](http://www.noisemachine.com/talk1/4.html)
* [Hugo Elias](http://freespace.virgin.net/hugo.elias/models/m_perlin.htm)
* [Paul Bourke](http://paulbourke.net/texture_colour/perlin/)
* [Developpez.com](http://khayyam.developpez.com/articles/algo/perlin/) (French)
