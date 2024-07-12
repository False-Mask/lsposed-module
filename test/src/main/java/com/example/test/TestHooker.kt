package com.example.test

import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage
import de.robv.android.xposed.XC_MethodHook

class TestHooker : IXposedHookLoadPackage {
    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {

        // XposedBridge.log("current:\n" + Thread.currentThread().stackTrace.joinToString("\n"))
        XposedBridge.log("test: 正在对 ${lpparam.packageName} 进行注入")
        val classloader = lpparam.classLoader
        val clz = classloader.loadClass("android.view.ViewRootImpl")
        val ws = classloader.loadClass("android.view.IWindowSession")
        val ct = classloader.loadClass("android.content.Context")
        val dp = classloader.loadClass("android.view.Display")
        clz.declaredConstructors
            .forEach {
                XposedBridge.log(it.toString())
            }
        // 测试 ViewRootImpl
        val vr = XposedHelpers.findAndHookConstructor(
            clz,
            ct,
            dp,
            ws,
            Boolean::class.java,
            object : XC_MethodHook() {
                override fun afterHookedMethod(param: MethodHookParam) {
                    XposedBridge.log("StackTrace:\n")
                    XposedBridge.log(Thread.currentThread().stackTrace.joinToString("\n"))
                }
            }
        )


        XposedBridge.log("test: ${lpparam.packageName} 注入完成")
    }
}