# elementary-desktop

This project was aiming to test if I could make a "missing" desktop for Elementary OS in QT, unfortunatelly it did not succeed in it due the
various problems related with QT (buggy support of drag and drop and several other minor things). But that might change in the future :)

However I've switched to Mint, and it's very unlikely that it's gonna be supported.

![Beautiful elementary os image](https://elementary.io/images/screenshots/desktop.jpg)

Problems 'ive faced so far:

1. Very bad support of drag & drop operations to the OS <-> QT, especially the ones using native file information (link or file object)
2. Few very tricky techniques to make QT app on top of the desktop and don't screw up everything in the mean time.
3. Crossplatform ways to obtain icons for the registered files / folders. Had to use external tools to obtain gnome theme name, and then looking for the icons in OS dependent folders (very very shity solution).

What's done:

1. Very cool icon rendering technique, which mimics OSX desktop along with rounded corners selection around the text under the icon.

Besides that everything is pretty straight forward, use file watcher to watch for the changes in the desktop directory and draw icons according to the grid keeping in mind font size, icon size, grid size and so on so on. Thing itself looks very impressive to me in the terms how easy and quick you can make something that looks and behaves "almost" like a native desktop application. However the bad part is that some minor issues / inconviniences makes it totally unusable in real life.
