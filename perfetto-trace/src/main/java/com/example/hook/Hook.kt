package com.example.hook

import android.os.Process
import android.view.View
import android.view.ViewGroup
import androidx.core.view.children
import com.wy.lib.wytrace.ArtMethodTrace
import dalvik.system.BaseDexClassLoader
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage

class Hook : IXposedHookLoadPackage {

    private val TAG = "perfetto-trace"

    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {
        XposedBridge.log("${TAG}: 正在对 ${lpparam.packageName} 进行注入")
        val classloader = lpparam.classLoader as BaseDexClassLoader
        // val artTrace = classloader.loadClass("com.wy.lib.wytrace.ArtMethodTrace")
        // XposedHelpers.callStaticMethod(artTrace, "methodHookStart", "android.view.Choreographer.FrameCallback.doFrame", Process.myTid(), 5, true)
        XposedBridge.log("classloader:" + classloader.toString())
        XposedBridge.log("Hook:" + Hook::class.java.classLoader.toString())
        XposedBridge.log("Main:" + MainActivity::class.java.classLoader.toString())
        XposedBridge.log("ArtHook:" + ArtMethodTrace::class.java.classLoader.toString())


        ArtMethodTrace.methodHookStart(
            "android.view.Choreographer.doFrame",
            Process.myTid(),
            Int.MAX_VALUE,
            true
        )

        XposedBridge.log("PId:" + Process.myPid())

        XposedBridge.log("${TAG}: ${lpparam.packageName} 注入完成")
    }


}