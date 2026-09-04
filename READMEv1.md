# GS-CARD

A perfect way to execute your hyperspace jump quickly and safely (not guaranteed).

![Logo](https://github.com/johnsnowdies/gs-card/blob/574a5a96de226de081c2c8692de4733a60df5b44/docs/logo.png)

# Build requirements

This program requires Borland TURBO C compiler version 1.0 to be built. The best way to build is to use the IDE provided by Borland:

1. Specify the project file CARD.PRJ.
2. Ensure the SYSTEM.SOL file exists in the root folder.
3. Build and run.

# Release and Usage Notes

To be frank, this software is somewhat outdated (by about 5000 years), so you need to make sure to obtain an actual SYSTEM.SOL file and store it in the same folder where CARD.EXE is located.

Using the SYSTEM.SOL provided in this repo can be dangerous, as almost all the old imperial navigation beacons aren't functioning anymore.

*The best way to run this app is using DosBox (in Pentium III mode).*

Upon the first launch, you will see GS-CARD calculating hyper-threads:

![Loading screen](https://github.com/johnsnowdies/gs-card/blob/4fcb22566115bc3c12819bd08fcd49fb6c3faeb2/docs/1.gif)

The next screen will show the galaxy map provided in the SYSTEM.SOL file:

![Main screen](https://github.com/johnsnowdies/gs-card/blob/4fcb22566115bc3c12819bd08fcd49fb6c3faeb2/docs/2.gif)

You can zoom in using the F3/F4 keys to view the solar indexes.

You can check the calculated possible threads using the F6 key:

![Threads screen](https://github.com/johnsnowdies/gs-card/blob/4fcb22566115bc3c12819bd08fcd49fb6c3faeb2/docs/3.gif)

To calculate a path and execute a hyperspace jump, press F7 and enter the START and END solar indexes:

![Path screen](https://github.com/johnsnowdies/gs-card/blob/4fcb22566115bc3c12819bd08fcd49fb6c3faeb2/docs/4.gif)

The path will be calculated and uploaded to your spaceship's hyperdrive engine.

P.S. I've had requests to remove this outdated advertisement, but it's not possible. Just ignore it, as all these G-Astro Network addresses don't work anymore.

# What the 🚀 

This app was made for fun! My friends and I were playing a tabletop role-playing game about a distant dystopian space future. We used this app as a galaxy map where the plot of this game unfolded.

Our story revolved around a ruined human empire, whose technology was terribly outdated but still useful. That's why I used MS-DOS and the Turbo C compiler from 1989. I didn't want to mess with DOS extenders (like DOS/4GW), so the entire app fits within 64kb of RAM.



