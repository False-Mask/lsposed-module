package com.example.hook.ui

import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.Settings
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import com.example.hook.databinding.ActivityMainBinding
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.launch

class MainActivity : AppCompatActivity() {

    companion object {
        private const val TAG = "MainActivity"
    }

    private val binding :ActivityMainBinding by lazy {
        ActivityMainBinding.inflate(layoutInflater)
    }

    private val vm by lazy {
        ViewModelProvider(this)[MainVM::class.java]
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        ViewCompat.setOnApplyWindowInsetsListener(binding.main) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
        initValue()
        observe()

    }

    private fun initValue() {
        vm.initConfig()
    }

    private fun observe() {
        lifecycleScope.launch {
            vm.config.collectLatest {
                // set optimized mode
                val optimizedMode = it.deOptimizedMode
                when (optimizedMode) {
                    0 -> {
                        binding.rgOptimizeMode.check(com.example.hook.R.id.btn_none)
                    }
                    1 -> {
                        binding.rgOptimizeMode.check(com.example.hook.R.id.btn_deoptimize_all)
                    }
                    2 -> {
                        binding.rgOptimizeMode.check(com.example.hook.R.id.btn_deoptimize_boot_image)
                    }
                }
            }
        }

        binding.rgOptimizeMode.setOnCheckedChangeListener { group, checkedId ->
            val config = vm.getValue()
            when(checkedId) {
                com.example.hook.R.id.btn_none -> {
                    config.deOptimizedMode = 0
                }
                com.example.hook.R.id.btn_deoptimize_all -> {
                    config.deOptimizedMode = 1
                }
                com.example.hook.R.id.btn_deoptimize_boot_image -> {
                    config.deOptimizedMode = 2
                }
            }
            Log.d(TAG, "changed")
            val value = config.copy()
            vm.saveConfig(value)
            vm.setConfig(value)
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            if (!Environment.isExternalStorageManager()) {
                val intent = Intent(
                    Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
                intent.data = Uri.parse("package:$packageName")
                startActivity(intent)
            }
        }

    }
}