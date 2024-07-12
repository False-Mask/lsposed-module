package com.example.view_tree

import android.view.View
import android.view.ViewGroup
import androidx.core.view.children
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage

class Cal : IXposedHookLoadPackage {

    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {
        XposedBridge.log("cal: 正在对 ${lpparam.packageName} 进行注入")
        val classloader = lpparam.classLoader
        val clz = classloader.loadClass("android.view.ViewRootImpl")

        val vr = XposedHelpers.findAndHookMethod(
            clz,
            "performTraversals",
            object : XC_MethodHook() {
                override fun afterHookedMethod(param: MethodHookParam) {
                    val thisObj = param.thisObject
                    val mViewField = clz.getDeclaredField("mView").apply {
                        isAccessible = true
                    }
                    val mView = mViewField.get(thisObj) as? View
                    if(mView != null) {
                        val map = mutableMapOf<Int,MutableList<Pair<String, Int>>>()
                        calculate(mView, 0, map)
                        sort(map)
                        XposedBridge.log("Start Tracing View Hierarchy")
                        printView(map)
                        XposedBridge.log("End   Tracing View Hierarchy")
                    }

                }
            }
        )


        XposedBridge.log("cal: ${lpparam.packageName} 注入完成")
    }

    private fun printView(map: MutableMap<Int, MutableList<Pair<String, Int>>>) {
        val size = map.size
        var count = 0.0
        for (i in 0 until size ) {
            val k = i
            val v = map[k]!!
            val strBuilder = StringBuilder("$k")
            var currentPercentage = map[i]!!.size / map[0]!![0].second.toFloat() * 100
            count += currentPercentage
            strBuilder.append("\t${String.format("%.2f", currentPercentage)}%")
            strBuilder.append("\t${String.format("%.2f", count)}%")
            for(views in v) {
                strBuilder.append("\t${views.first}(${views.second})")
            }
            XposedBridge.log(strBuilder.toString())
        }
    }

    fun sort(map: MutableMap<Int, MutableList<Pair<String, Int>>>) {
        for((_,v) in map) {
            v.sortByDescending { it.second }
        }
    }

    fun calculate(mView: View, dep: Int, map: MutableMap<Int, MutableList<Pair<String, Int>>>): Int {
        var childCountAll = 1
        if(mView is ViewGroup) {
            for(child in mView.children) {
                childCountAll += calculate(child, dep + 1, map)
            }
        }
        if(!map.containsKey(dep)) {
            map[dep] = mutableListOf()
        }
        map[dep]!!.add(Pair(mView.javaClass.simpleName, childCountAll))
        return childCountAll
    }
}