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

Feel free to borrow from this sketch if it helps you.

Have fun!

