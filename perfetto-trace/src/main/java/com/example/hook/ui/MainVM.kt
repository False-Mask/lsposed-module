package com.example.hook.ui

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.hook.Config
import com.example.hook.utils.ConfigSaver
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

class MainVM : ViewModel() {

    companion object {
        private const val TAG = "MainVM"
    }

    private val _config by lazy {
        MutableStateFlow(Config())
    }
    val config = _config.asStateFlow()

    fun initConfig() {
        viewModelScope.launch(Dispatchers.IO) {
            try {
                val config = ConfigSaver.getValue()
                _config.emit(config)
            } catch (e: Throwable) {
                // error
                _config.emit(Config())
            }
        }
    }

    fun getValue(): Config {
        return _config.value
    }

    fun setConfig(value: Config) {
        viewModelScope.launch {
            _config.emit(value)
        }

    }

    fun saveConfig(config: Config) {
        viewModelScope.launch(Dispatchers.IO)  {
            try{
                ConfigSaver.saveValue(config)
                Log.d(TAG,"saveConfig success")
            } catch (e: Throwable) {
                Log.e(TAG, "saveConfig failed: " + Log.getStackTraceString(e))
            }
        }
    }

    fun startServer() {

    }

}