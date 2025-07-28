package com.example.hook.utils

import android.os.Environment
import com.example.hook.Config
import java.io.File

object ConfigSaver {

    private val SP = "SP_SAVER"

    private const val DEOPTIMIZED_KEY = "deoptimized"
    private val PREFIX = File(Environment.getExternalStorageDirectory(),".perftto-trace")
    private val file = File(PREFIX,"$SP.xml")

    fun saveValue(config: Config) {
        if (!file.exists()) {
            val parent = file.parentFile
            if (parent?.exists() == false) {
                parent.mkdirs()
            }
            file.createNewFile()
        } else {
            if (file.isDirectory) {
                file.deleteRecursively()
            }
        }
        file
            .bufferedWriter()
            .use {
                it.write(gson.toJson(config))
            }
    }

    fun getStringValue(): String {
        return file
            .bufferedReader()
            .use {
                it.readLines().joinToString("\n")
            }
    }

    fun getValue(): Config {
        return gson.fromJson(getStringValue(), Config::class.java)
    }

}