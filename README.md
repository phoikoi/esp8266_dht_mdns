# ESP8266 DHT/mDNS/HTTP client demo

I cobbled this sketch together from several different library
examples, because I needed to gather data from a DHT22
temperature/humidity sensor, then send it off to a server
on my local network.  The network has no proper DNS server
for internal names, only mDNS (aka "Bonjour",) so I also
needed to figure out how to resolve a DNS A record via
the mDNS library.  It turns out to be somewhat of an odd
process.

I also then use the HTTP client library to send
the data via GET parameters.  (It's a lot simpler than
doing a POST request, which is the Proper Way To
Do Things.)

> Note: this sketch uses the `ESP.deepSleep()` call,
> which necessitates connecting the RESET pin to the
> DTR / GPIO16 pin in order for the ESP8266 to wake
> itself after the given interval.  The problem is
> that GPIO16 is also apparently used by the USB
> connector, such that the board can't be
> reprogrammed while that connection is wired.
> So make sure to disconnect it when you want to
> upload a sketch.

Feel free to borrow from this sketch if it helps you.

Have fun!

