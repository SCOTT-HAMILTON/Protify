<p align="center">
    <img width="400" alt="Protify Logo" src="https://raw.githubusercontent.com/SCOTT-HAMILTON/Protify/master/icons/finished.svg">
</p>

<p align="center">
      <a href="https://scott-hamilton.mit-license.org/"><img alt="MIT License" src="https://img.shields.io/badge/License-MIT-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
</p>

<h1 align="center">Protify - Leave your computer and let it notify your phone </h1>

<p align="center">
  <img width="600"
       alt="Protify - App Demo"
       src="https://media.giphy.com/media/w8kc45y4Q3hgqNa99V/giphy.gif">
</p>

## About
Protify lets you choose long-running processes on your computer and it will watch them for you. And you'll receive a notification on you android mobile phone each time one of them has terminated.

## How does it work
To work with your android phone, use the [Protifydroid app](https://github.com/SCOTT-HAMILTON/Protifydroid). Protify will only work if both your mobile and your computer are connected to the same local network such has your home's wifi. It uses mDNS under the hood.

## Building
Protify is configured with qmake, simply open it in QtCreator or run qmake manually

## Dependencies
Protify depends on
 - [qt-qml-models](http://gitlab.unique-conception.org/qt-qml-tricks/qt-qml-models) for the QML ListViews
 - [QtZeroConf](https://github.com/jbagg/QtZeroConf) for mDNS
 - [libproc](https://github.com/reyreaud-l/libproc) for listing running processes

## License
Protify is released under the well known [MIT License](https://scott-hamilton.mit-license.org/)
