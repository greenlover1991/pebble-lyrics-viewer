# pebble-lyrics-viewer
Simple scrollable lyrics viewer for Pebble

- displays current track info (title, artist, album)
- retrieves lyrics from an API and displays in a ScrollLayer
* ANDROID only
* Requires Automate Flow: - https://llamalab.com/automate/community/flows/16413
* My First Pebble app

Instructions:
1. Install Automate and download the Automate Flow
2. Start the Flow
3. Play Spotify song
4. Launch watch app
5. Lyrcis should be shown

CREDITS
* blog.ithasu.org/2016/01/using-llamalabs-automate-as-a-pebble-time-companion-app/ - Write up on the Intent broadcasted by Pebble App
* https://github.com/rhnvrm/lyric-api - Web API used to fetch lyrics

TODO:
* auto scrolling based on song length and current
* add time display on the top bar in watch app
