package com.example.codelocator

import android.app.Application
import android.content.Context
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage
import com.bytedance.tools.codelocator.CodeLocator

class CodeLocatorInjector : IXposedHookLoadPackage {
    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {
        XposedBridge.log("正在对 ${lpparam.packageName} 进行注入")
        XposedHelpers.findAndHookMethod(Application::class.java, "attach", Context::class.java,
            object : XC_MethodHook() {
                override fun afterHookedMethod(param: MethodHookParam) {
                    val clz = CodeLocator::class.java

                    val init = clz.getDeclaredMethod("init", Application::class.java).also {
                        it.invoke(null,param.thisObject as Application)
                    }

                    val registerReceiver = clz.getDeclaredMethod("registerReceiver").also {
                        it.isAccessible = true
                        it.invoke(null)
                    }
                }
            })
        XposedBridge.log("${lpparam.packageName} 注入成功")
    }

}