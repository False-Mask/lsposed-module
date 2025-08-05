package com.example.hook.hook

import android.app.Application
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Handler
import android.os.Looper
import android.os.Process
import android.util.Log
import androidx.core.content.ContextCompat
import com.example.hook.Config
import com.example.hook.server.port
import com.example.hook.utils.ConfigSaver
import com.example.hook.utils.gson
import com.wy.lib.wytrace.ArtMethodTrace
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.IXposedHookZygoteInit
import de.robv.android.xposed.XC_MethodHook
import de.robv.android.xposed.XposedBridge
import de.robv.android.xposed.XposedHelpers
import de.robv.android.xposed.callbacks.XC_LoadPackage
import okhttp3.Request

class Hook : IXposedHookZygoteInit {

    private val TAG = "perfetto-trace"

    private val handler by lazy {
        Handler(Looper.getMainLooper())
    }

    private val  config by lazy {
        ConfigSaver.getValue()
    }

    lateinit var pkg: String
    private var pid: Int = -1



//    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam?) {
//        XposedBridge.log("modulePath=" + lpparam?.packageName)
//        XposedBridge.log("startSystemServer=" + lpparam?.appInfo)
//        when(1) {
//            0 -> {
//                XposedBridge.log("deOptimizeAll Nothing !")
//            }
//            1 -> {
//                XposedBridge.log("deOptimizeAll !!!")
//                ArtMethodTrace.deoptimizedEverything()
//            }
//            2 -> {
//                XposedBridge.log("deOptimized Boot Image !!!")
//            }
//        }
//
//        XposedHelpers.findAndHookMethod(
//            Application::class.java, "attach", Context::class.java,
//            object : XC_MethodHook() {
//                override fun afterHookedMethod(param: MethodHookParam) {
//                    doHookInternal(param.thisObject as Application)
//                }
//            })
//
//    }

    private fun doHookInternal(app: Application) {
        if (pid == -1) {
            pid = Process.myPid()
        } else {
            return
        }

        ArtMethodTrace.methodHookStart("", Process.myTid() , Int.MAX_VALUE, false)

        ContextCompat.registerReceiver(app, object : BroadcastReceiver() {
            override fun onReceive(context: Context?, intent: Intent?) {
                XposedBridge.log("received trace req")
                val ext = intent?.extras ?: return
                val what = ext.getString("what")
                when(what) {
                    "0" -> {
                        XposedBridge.log("start Tracing " + app.packageName)
                        ArtMethodTrace.startTrace()
                    }
                    "1" -> {
                        XposedBridge.log("end Tracing " + app.packageName)
                        ArtMethodTrace.endTrace()
                    }
                }

            }

        },IntentFilter("com.intent.action.TRACE"), ContextCompat.RECEIVER_EXPORTED)

//        var conf: Config = Config()
//        runCatching {
//            val response = client.newCall(Request.Builder()
//                .url("http://127.0.0.1:$port/config")
//                .build())
//                .execute()
//            val json = response.body.toString()
//            if (json.isNotEmpty()) {
//                conf = gson.fromJson(json, Config::class.java)
//            }
//        }.onFailure {
//            XposedBridge.log("getConfig Error: " + Log.getStackTraceString(it))
//        }
//        ArtMethodTrace.foreInterpretor()
    }

    override fun initZygote(startupParam: IXposedHookZygoteInit.StartupParam?) {
        XposedBridge.log("modulePath=" + startupParam?.modulePath)
        XposedBridge.log("startSystemServer=" + startupParam?.startsSystemServer)
        when(1) {
            0 -> {
                XposedBridge.log("deOptimizeAll Nothing !")
            }
            1 -> {
                XposedBridge.log("deOptimizeAll !!!")
                ArtMethodTrace.deoptimizedEverything()
            }
            2 -> {
                XposedBridge.log("deOptimized Boot Image !!!")
            }
        }

        XposedHelpers.findAndHookMethod(
            Application::class.java, "attach", Context::class.java,
            object : XC_MethodHook() {
                override fun afterHookedMethod(param: MethodHookParam) {
                    doHookInternal(param.thisObject as Application)
                }
            })
    }


}