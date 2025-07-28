package com.example.hookee

import android.app.Application
import android.content.Context
import android.os.Process
import com.wy.lib.wytrace.ArtMethodTrace

class App : Application() {

    override fun attachBaseContext(base: Context?) {
        super.attachBaseContext(base)
        ArtMethodTrace.methodHookStart("", Process.myTid(), Int.MAX_VALUE, true)
    }

}
