# TOTP
```
/*
 * Default power on with left button press
 * Default power off with left button 6 sec press
 * Main button write otp and switch off the device
 * Right button change account
 * Right button long press enter config mode creating a wifi with a captive portal to configure the device
 */
```

credentials json

```
{
    "list": [
        {
        	"name": "VPN",
        	"secret" : "2IGPMWLF2FHA8DJW6KKQ724RL",
        	"isOTP": true
        },
        {
        	"name": "AWS",
        	"secret" : "JD93UMBII26NOYEDR7WE7WXJQOJTVKXZZZCNKL34F5UFN5CFGJLCR4IVEJVFI0IO",
        	"isOTP": true
        },
        {
        	"name": "GitHub",
        	"secret": "supersecretpassword",
        	"isOTP": false
        }        
    ]
}
```

if captive portal doesn't open the default server ip is 192.168.4.1
