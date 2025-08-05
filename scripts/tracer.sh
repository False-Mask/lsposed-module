function startTrace() {
 adb shell am broadcast -a "com.intent.action.TRACE" --es "what" "0"
}

function stopTrace() {
 adb shell am broadcast -a "com.intent.action.TRACE" --es "what" "1"
}

function parseTrace() {
  adb pull /data/user/0/com.zhiliaoapp.musically.go/.com.zhiliaoapp.musically.go tmp
  python3 scripts/python/parser.py
}