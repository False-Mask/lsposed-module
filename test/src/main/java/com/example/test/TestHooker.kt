package com.example.test

import android.app.Application
import android.os.Debug
import android.os.Handler
import android.os.Looper
import android.view.View
import android.view.ViewGroup
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage
import de.robv.android.xposed.XC_MethodHook
import kotlin.concurrent.thread

class TestHooker : IXposedHookLoadPackage {

    var isTracing = false

    private var all: Int = 0
    private val map = mutableMapOf<String, Int>()

    private val handler by lazy {
        Handler(Looper.getMainLooper())
    }


    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {

//        if(!lpparam.packageName.equals("com.zhiliaoapp.musically.go")) {
//            return
//        }

        XposedBridge.log("Test: begin")

        val classloader = lpparam.classLoader

        XposedHelpers.findAndHookMethod(Application::class.java, "onCreate", object :
            XC_MethodHook() {

            override fun beforeHookedMethod(param: MethodHookParam) {

                Debug.startMethodTracing()
                XposedBridge.log("Trace Begin!")


                thread {
                    Thread.sleep(3000)
                    Debug.stopMethodTracing()
                    XposedBridge.log("Trace End!")
                }

//                val C5Ek = classloader.loadClass("X.5Ek")
//                val jsonObj = classloader.loadClass("org.json.JSONObject")
//                val C4uN = classloader.loadClass("X.4uN")
//
//                C4uN.getDeclaredMethod("a")
//                    .isAccessible

//                XposedHelpers.findAndHookMethod(C5Ek, "onPlayStop",
//                    String::class.java,
//                    jsonObj,
//                    C4uN,
//                    object : XC_MethodHook() {
//
//                        override fun beforeHookedMethod(param: MethodHookParam?) {
//                            XposedBridge.log("stop")
//                            XposedBridge.log(Thread.currentThread().stackTrace.joinToString("\n"))
//                        }
//
//                    })
//
//                XposedHelpers.findAndHookMethod(C5Ek, "onPlayRelease",
//                    String::class.java,
//                    object : XC_MethodHook() {
//
//                        override fun beforeHookedMethod(param: MethodHookParam?) {
//                            XposedBridge.log("release")
//                            XposedBridge.log(Thread.currentThread().stackTrace.joinToString("\n"))
//                        }
//
//                    })


            }

        })


        XposedBridge.log("Test: end")


    }

    private fun printRemoveTime(classloader: ClassLoader) {

        val C4N5 = classloader.loadClass("X.4N5")
        XposedHelpers.findAndHookMethod(C4N5, "removeView", View::class.java, object :
            XC_MethodHook() {

            override fun beforeHookedMethod(param: MethodHookParam) {
                replaceHookedMethod(param)
            }

            fun replaceHookedMethod(param: MethodHookParam): Any {
                XposedBridge.log("=================================")
                XposedBridge.log(Thread.currentThread().stackTrace.joinToString("\n"))
                XposedBridge.log("=================================")
//                        if(!isTracing) {
//                            isTracing = true
//                            Debug.startMethodTracingSampling(null, 1024* 1024 * 1024, 10)
//                            thread {
//                                Thread.sleep(3000)
//                                Debug.stopMethodTracing()
//                                isTracing = false
//                                XposedBridge.log("trace end")
//                            }
//                        }

                return Unit
            }


        })
    }

    private fun ttliteTextViews(classloader: ClassLoader) {
        val C1NL = classloader.loadClass("X.1NL")
        val str = classloader.loadClass("java.lang.String")
        val bufferType = classloader.loadClass("android.widget.TextView\$BufferType")
        XposedHelpers.findAndHookMethod(C1NL,
            "setText",
            CharSequence::class.java,
            bufferType,
            object : XC_MethodHook() {
                override fun afterHookedMethod(param: MethodHookParam) {
                    Thread.currentThread().stackTrace.forEach {
                        if (it.toString().contains("X.49g")) {
                            XposedBridge.log("error!!")
                            Thread.sleep(100)
                        }
                    }
                }
            })

    }


    private fun insViews(classloader: ClassLoader) {
        val viewgroup = classloader.loadClass("android.view.ViewGroup")
        val roundedCorner =
            classloader.loadClass("com.instagram.common.ui.widget.imageview.RoundedCornerImageView")
        XposedHelpers.findAndHookMethod(viewgroup, "removeView", View::class.java, object :
            XC_MethodHook() {

            override fun beforeHookedMethod(param: MethodHookParam) {
                if (param.args[0] == null) {
                    return
                }
                val view = param.args[0] as View
                XposedBridge.log(view.toString())
                if (roundedCorner.isInstance(view)) {
//                    XposedBridge.log(roundedCorner.toString())
//                    XposedBridge.log(view.toString())
//                    val newView = view.asViewGroup().getChildAt(0)
//                        .asViewGroup().getChildAt(0)
//                    XposedBridge.log(newView.toString())
                }
            }

        })
    }

    fun View.asViewGroup(): ViewGroup {
        return this as ViewGroup
    }


    public fun ttLiteViews(classloader: ClassLoader) {
        val handlerClz: Class<*> = classloader.loadClass("android.os.Handler")
        val messageQueue = classloader.loadClass("android.os.MessageQueue")
        val msg = classloader.loadClass("android.os.Message")
        val C4N5 = classloader.loadClass("X.4N5")
        XposedHelpers.findAndHookMethod(C4N5, "removeView", View::class.java, object :
            XC_MethodHook() {

            override fun beforeHookedMethod(param: MethodHookParam) {
                val view = param.args[0] as View
                val viewStr = str(view.toString())
                map[viewStr] = (map[viewStr] ?: 0) + 1
                XposedBridge.log("=== begin ===")
                map.forEach { t, u ->
                    XposedBridge.log("$t\t:\t$u")
                }
                XposedBridge.log("=== end   ===")
            }

        })
    }

    private fun str(string: String): String {
        val idx = string.indexOf("{")
        if (idx < 0) {
            return string
        }
        return string.substring(0, idx)
    }
}