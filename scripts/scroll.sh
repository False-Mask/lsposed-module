for i in {1..1000}
do
  adb shell input swipe 500 2000 500 0 50;
  echo "第 $i 个 Item 滑动完成";
done
