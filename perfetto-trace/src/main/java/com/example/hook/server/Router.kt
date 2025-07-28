package com.example.hook.server

import android.util.Log
import com.example.hook.utils.ConfigSaver
import io.ktor.server.application.*
import io.ktor.server.response.*
import io.ktor.server.routing.*

fun Application.configureRouting() {
    routing {
        get("/config") {
            val value = ConfigSaver.getStringValue()
            call.respondText(value)
            Log.e("LSPosed", value)
        }
    }
}