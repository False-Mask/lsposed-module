import org.jetbrains.kotlin.fir.declarations.builder.buildField

plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.jetbrains.kotlin.android)
}

val pkg = "com.example.hook"

android {
    namespace = pkg
    compileSdk = 33

    defaultConfig {
        applicationId = pkg
        minSdk = 26
        targetSdk = 33
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    packaging {
        resources {
            excludes += listOf("META-INF/INDEX.LIST", "META-INF/io.netty.versions.properties")
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            buildConfigField("String","PKG", "\"$pkg\"")
        }

        viewBinding {
            enable = true
        }

        debug {
            buildConfigField("String", "PKG", "\"$pkg\"")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    buildFeatures {
        prefab = true
        buildConfig = true
        aidl = true
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    sourceSets {
        getByName("main") {
            aidl {
                srcDirs("src/main/aidl")
            }
        }
    }
}

dependencies {

    compileOnly(libs.api)

    val lifecycleVersion = "2.4.0"
    implementation("androidx.lifecycle:lifecycle-viewmodel-ktx:${lifecycleVersion}")
    implementation("androidx.lifecycle:lifecycle-runtime-ktx:${lifecycleVersion}")


    implementation("com.bytedance.android:shadowhook:1.0.9")
    implementation("com.google.code.gson:gson:2.13.1")
    // implementation("com.github.wuyouuuu:wytrace:1.0.1")
    implementation(project(":wytrace"))

    implementation("com.squareup.okhttp3:okhttp:4.0.0")
    // ktor client
//    implementation(libs.ktor.client.core)
//    implementation(libs.ktor.client.okhttp)
    implementation(libs.ktor.logging)
    implementation(libs.sl4j)
    // ktor server
    implementation(libs.ktor.server.core)
    implementation(libs.ktor.server.netty)

    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.activity)
    implementation(libs.androidx.constraintlayout)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}