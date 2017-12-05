SimpleMesh
==========
SimpleMesh is a simple way to build mash networks using and Arduino and RFM69 chips. It uses ECDH to secure the network. SimpleMesh is as the name suggests very simple. It doesn't keep a map of the network thus repeats all messages it receives and is not suggested for big networks or high amounts of traffic.

[slideshow](https://docs.google.com/presentation/d/e/2PACX-1vSuKsd36Ef2N6bLRBq_TvwxMUaBEAJs5MbuhF40pO_gngQPLbqZUDFlH0bbMkvVuE33nmy2TxCoh3P1/embed?start=false&loop=false&delayms=3000)

## Package
SimpleMesh uses the RFM69's built in capabilities for addressing but has to add some extra 2 bytes to word correct
```
 xxxx xxxx xxxx xxxx ------------
 |senderID|destinationID|sequence| data
```
senderID is the real sender, it is used to keep the seq number correct. If the sequence number is 0 it means a reset of counting and will always be repeated.

## Background
SimpleMesh is developed for an international collaboration between TH Köln and Thomas More Geel. Since we have never seen a solution like this we've decided to open source our work under the GPL License following the libraries we use.

## Project by
- Ben De Breuker
- Maartje Eyskens
- Kristie Lim
