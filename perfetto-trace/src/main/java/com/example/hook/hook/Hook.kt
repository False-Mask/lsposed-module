package com.example.hook.hook

import android.os.Build
import android.os.Handler
import android.os.Looper
import android.os.Process
import android.util.Log
import androidx.annotation.RequiresApi
import com.example.hook.Config
import com.example.hook.server.host
import com.example.hook.server.port
import com.example.hook.utils.ConfigSaver
import com.example.hook.utils.gson
import com.wy.lib.wytrace.ArtMethodTrace
import de.robv.android.xposed.IXposedHookZygoteInit
import de.robv.android.xposed.XposedBridge
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

    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    override fun initZygote(lpparam: IXposedHookZygoteInit.StartupParam) {
        XposedBridge.log("modulePath=" + lpparam.modulePath)
        XposedBridge.log("startSystemServer=" + lpparam.startsSystemServer.toString())
        if (pid == -1) {
            pid = Process.myPid()
        } else {
            return
        }
        var conf: Config = Config()
        runCatching {
            val response = client.newCall(Request.Builder()
                .url("http://127.0.0.1:$port/config")
                .build())
                .execute()
            val json = response.body.toString()
            if (json.isNotEmpty()) {
                conf = gson.fromJson(json, Config::class.java)
            }
        }.onFailure {
            XposedBridge.log("getConfig Error: " + Log.getStackTraceString(it))
        }
//        ArtMethodTrace.foreInterpretor()

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
        ArtMethodTrace.methodHookStart("", Process.myTid() , Int.MAX_VALUE, false)

    }


}