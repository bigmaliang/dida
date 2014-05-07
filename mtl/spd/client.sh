#!/usr/bin/expect -f

set passwd 123123
set index [lindex $argv 0]
set users(1) spdbeijing
set users(2) spdshanghai
set users(3) spdguangzhou
set users(4) spdshengzhen
set users(5) spdwuhan

#set users "spdshanghai spdbeijing spdguangzhou spdshengzhen spdwuhan"

spawn su $users($index) -
expect {
   "Password:" {
      send "${passwd}\r"
      }
   "yes/no)?" {
      send "yes\r"
      }
 }    

expect "@"
send  "google-chrome &\r"
#send "ls -l\r"

#interact
set bCtrlC false
interact {
    \003 {
        set bCtrlC true
        return
    }
}
