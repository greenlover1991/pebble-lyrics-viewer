//  APP messages
Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS READY");
    
//     var lyrics = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
//       + "Nam quam tellus, fermentu  m quis vulputate quis, vestibulum interdum sapien.\n"
//       + "Vestibulum lobortis pellentesque pretium. Quisque ultricies purus e  u orci convallis lacinia.\n"
//       + "Cras a urna mi. Donec convallis ante id dui dapibus nec ullamcorper erat egestas.\n"
//       + "Aenean a m  auris a sapien commodo lacinia. Sed posuere mi vel risus congue ornare.\n"
//       + "Curabitur leo nisi, euismod ut pellentesque se  d, suscipit sit amet lorem.\n"
//       + "Aliquam eget sem vitae sem aliquam ornare.\n"
//       + "In sem sapien, imperdiet eget pharetra a, lacin  ia ac justo.\n"
//       + "Suspendisse at ante nec felis facilisis eleifend.";
    
//     Pebble.sendAppMessage({'LYRICS': lyrics});
    var artist = "Sia";
    var title = "Titanium";
    var url = "https://lyric-api.herokuapp.com/api/find/" 
      + encodeURIComponent(artist) + "/"
      + encodeURIComponent(title);
    get(1, url, function(reqCode, json) {
      Pebble.sendAppMessage({'LYRICS': json.lyric});
    });
    
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('App message received: ' + JSON.stringify(e));
    
    // FIXME when received Android broadcast, GET lyrics
    // Pebble.sendAppMessage(json);
  }
);

/** HTTP utils */
const MAX_TIMEOUT = 5000; // 5 seconds to connect
function get(reqCode, url, cbSuccess, cbFail, cbError) {
  var req = new XMLHttpRequest();
  req.onload = function() {
    console.log(this.responseURL + " responded: " + this.statusText);
    if (this.status == 200) {
      if (cbSuccess != undefined) {
        cbSuccess(reqCode, JSON.parse(this.responseText));
        return;
      }
    } else {
      if (cbFail != undefined) {
        cbFail(reqCode, this.status, JSON.parse(this.responseText));
        return;
      }
    }
  };
  req.onerror = function(e) {
    console.log("Error connecting" + this.responseURL + ". " + e);
    if (cbError != undefined) {
      cbError(reqCode, e);
    }
  };
  req.ontimeout = function(e) {
    console.log("(" + this.timeout + "ms) Timeout connecting" + this.responseURL + ". " + e);
    if (cbError != undefined) {
      cbError(reqCode, e);
    }
  };
  req.timeout = MAX_TIMEOUT;
  req.open("GET", url);
  req.send(); 
}