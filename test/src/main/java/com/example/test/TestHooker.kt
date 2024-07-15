package com.example.test

import android.app.Application
import android.os.Debug
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XC_MethodReplacement
import kotlin.concurrent.thread

class TestHooker : IXposedHookLoadPackage {
    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {
        val classloader = lpparam.classLoader
        val replacement = object : XC_MethodHook() {

            override fun beforeHookedMethod(param: MethodHookParam) {
                param.result = null

                Debug.startMethodTracingSampling(null, 1024 * 1024, 1000)
                XposedBridge.log(System.currentTimeMillis().toString() + ": startTrace")

                // XposedBridge.invokeOriginalMethod(param.method, param.thisObject, param.args)

                Debug.stopMethodTracing()
                XposedBridge.log(System.currentTimeMillis().toString() + ": endTrace")

//                thread {
//                    Thread.sleep(1000)
//                    Debug.stopMethodTracing()
//                    XposedBridge.log(System.currentTimeMillis().toString() + ": endTrace")
//                }
            }


        }

        XposedHelpers.findAndHookMethod(Application::class.java, "onCreate" , replacement)

//        thread {
//            XposedBridge.log("start!!")
//            Thread.sleep(5000)
//            XposedHelpers.findAndHookMethod(classloader.loadClass("X.QOZ"), "A01",Int::class.java , replacement)
//        }

    }
}