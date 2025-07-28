package com.example.view_tree

import android.annotation.SuppressLint
import android.view.SurfaceView
import android.view.View
import android.view.ViewGroup
import androidx.core.view.children
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage.LoadPackageParam

class SurfaceViewFinder(
    val params: LoadPackageParam
) : XC_MethodHook(), Hooker {

    @SuppressLint("PrivateApi", "DiscouragedPrivateApi")
    override fun afterHookedMethod(param: MethodHookParam) {
        val thisObj = param.thisObject
        val classloader = params.classLoader
        val clz = classloader.loadClass("android.view.ViewRootImpl")
        val mViewField = clz.getDeclaredField("mView").apply {
            isAccessible = true
        }
        val mView = mViewField.get(thisObj) as? View
        if(mView != null) {
            calculate(mView)
        }

    }

    private fun calculate(mView: View) {
        if(mView is ViewGroup) {
            for(child in mView.children) {
                calculate(child)
            }
        }

        if(mView is SurfaceView) {
            XposedBridge.log("find SurfaceView $mView")
        }

    }

    override fun hook() {
        XposedBridge.log("cal: 正在对 ${params.packageName} 进行注入")
        val classloader = params.classLoader
        val clz = classloader.loadClass("android.view.ViewRootImpl")
        val vr = XposedHelpers.findAndHookMethod(clz, "performTraversals", SurfaceViewFinder(params))
        XposedBridge.log("cal: ${params.packageName} 注入完成")
    }

}