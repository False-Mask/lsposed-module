plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.jetbrains.kotlin.android)
}

android {
    namespace = "com.example.hooktester"
    compileSdk = 33

    defaultConfig {
        applicationId = "com.example.hooktester"
        minSdk = 24
        targetSdk = 33
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        ndk {
            abiFilters += listOf("arm64-v8a", "armeabi-v7a")
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
//            arguments += listOf("-DANDROID_STL=c++_shared")
        }


    }


    buildFeatures {
        prefab = true
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
}

dependencies {

    implementation(libs.shadowhook)
    // https://mvnrepository.com/artifact/de.robv.android.xposed/api
    compileOnly("de.robv.android.xposed:api:82")
//    compileOnly("de.robv.android.xposed:api:82:sources")
    // https://mvnrepository.com/artifact/com.bytedance.tools.codelocator/codelocator-core
    implementation("com.bytedance.tools.codelocator:codelocator-core:2.0.4")

    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.activity)
    implementation(libs.androidx.constraintlayout)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}