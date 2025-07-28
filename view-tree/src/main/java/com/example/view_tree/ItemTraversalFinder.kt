package com.example.view_tree

import android.annotation.SuppressLint
import android.media.MediaDrm.ErrorCodes
import android.os.Process
import android.view.View
import android.view.ViewGroup
import androidx.core.view.children
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage

data class Data(
    val id: Int,
    val dep: Int,
    val name: String,
    val hashCode: Int,
    val toString: String,

)

class ItemTraversalFinder(
    val params: XC_LoadPackage.LoadPackageParam
) : XC_MethodHook(), Hooker {

    private var FILTER_VIEW_NAME = "QOU"


    @SuppressLint("PrivateApi")
    override fun hook() {
        XposedBridge.log("cal: 正在对 ${params.packageName} 进行注入")
        val classloader = params.classLoader
        val clz = classloader.loadClass("android.view.ViewRootImpl")
        val pkgName = params.packageName
        val pname = Process.myProcessName()
//        if (pname.contains(":") || pname.contains("com.google.android")) {
//            return
//        }
//        FILTER_VIEW_NAME = when (pkgName) {
//            "com.kwai.video" -> {
//                "SlidePlayItemRootFrameLayout"
//            }
//            "com.instagram.android" -> {
//                "QOU"
//            }
//            "com.zhiliaoapp.musically.go" -> {
//                "ZoomLayout"
//            }
//            else -> {
//                ""
//            }
//        }
        val vr =
            XposedHelpers.findAndHookMethod(clz, "performTraversals", ItemTraversalFinder(params))
        XposedBridge.log("cal: ${params.packageName} 注入完成")
    }


    @SuppressLint("PrivateApi", "DiscouragedPrivateApi")
    override fun afterHookedMethod(param: MethodHookParam) {
        val classloader = params.classLoader
        val clz = classloader.loadClass("android.view.ViewRootImpl")
        val thisObj = param.thisObject
        val mViewField = clz.getDeclaredField("mView").apply {
            isAccessible = true
        }
        val mView = mViewField.get(thisObj) as? View

        if (mView != null) {
            val views = findItemViewGroup(mView)
            XposedBridge.log("开始 dump View 数据")
            views.forEach { v->
                val res = mutableListOf<Data>()
                XposedBridge.log(v.hashCode().toString())
                XposedBridge.log("Start Tracing View Hierarchy")
                collectViews(v,0, res)
                printAllLines(res)
                XposedBridge.log("End   Tracing View Hierarchy")
            }
            XposedBridge.log("完成 dump View 数据")
        }

    }

    private fun printAllLines(res: MutableList<Data>) {
        XposedBridge.log("id,dep,name,hasCode")
        res.forEach {
            XposedBridge.log(StringBuilder().apply {
                append(it.id).append(",")
                    .append(it.dep).append(",")
                    .append(it.name).append(",")
                    .append(it.hashCode)
                    // .append(it.toString)
            }.toString()
            )
        }

    }

    private fun findItemViewGroup(v: View): List<ViewGroup> {
        val res = mutableListOf<ViewGroup>()
        val queue = ArrayDeque<View>()
        queue.addLast(v)
        while (queue.size != 0) {
            val node = queue.removeFirst()
            if (node is ViewGroup) {
                for (child in node.children) {
                    queue.addFirst(child)
                }
                if (node.toString().contains(FILTER_VIEW_NAME)) {
                    res.add(node)
                }
            }
        }

        return res
    }

    private fun collectViews(v:View,dep: Int, res: MutableList<Data>) {
        res.add(Data(res.size,dep, v::class.java.simpleName, v.hashCode(),v.toString()))
        if(v is ViewGroup) {
            for(view in v.children) {
                collectViews(view,dep + 1, res)
            }
        }
    }

    @SuppressLint("DefaultLocale")
    private fun printView(map: MutableMap<Int, MutableList<Pair<String, Int>>>) {
        val size = map.size
        var count = 0.0
        for (i in 0 until size) {
            val k = i
            val v = map[k]!!
            val strBuilder = StringBuilder("$k")
            var currentPercentage = map[i]!!.size / map[0]!![0].second.toFloat() * 100
            count += currentPercentage
            strBuilder.append("\t${String.format("%.2f", currentPercentage)}%")
            strBuilder.append("\t${String.format("%.2f", count)}%")
            for (views in v) {
                strBuilder.append("\t${views.first}(${views.second})")
            }
            XposedBridge.log(strBuilder.toString())
        }
    }

    private fun sortData(map: MutableMap<Int, MutableList<Pair<String, Int>>>) {
        for ((_, v) in map) {
            v.sortByDescending { it.second }
        }
    }

    private fun calculate(
        mView: View,
        dep: Int,
        map: MutableMap<Int, MutableList<Pair<String, Int>>>
    ): Int {
        var childCountAll = 1
        if (mView is ViewGroup) {
            for (child in mView.children) {
                childCountAll += calculate(child, dep + 1, map)
            }
        }
        if (!map.containsKey(dep)) {
            map[dep] = mutableListOf()
        }
        map[dep]!!.add(Pair(mView.javaClass.simpleName, childCountAll))
        return childCountAll
    }

}