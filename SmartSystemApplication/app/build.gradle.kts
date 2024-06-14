plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.lolicononline.smartsystemapplication"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.lolicononline.smartsystemapplication"
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = true
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    buildFeatures {
        viewBinding = true
    }
    packaging {
        resources {
            excludes += listOf("META-INF/INDEX.LIST", "META-INF/io.netty.versions.properties")
        }
    }
}

dependencies {

    implementation("androidx.core:core-ktx:1.13.1")
    implementation("androidx.appcompat:appcompat:1.7.0")
    implementation("com.google.android.material:material:1.12.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    implementation("androidx.lifecycle:lifecycle-livedata-ktx:2.8.2")
    implementation("androidx.lifecycle:lifecycle-viewmodel-ktx:2.8.2")
    implementation("androidx.navigation:navigation-fragment-ktx:2.7.7")
    implementation("androidx.navigation:navigation-ui-ktx:2.7.7")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")

    implementation("com.github.Jay-Goo:RangeSeekBar:v3.0.0")

    implementation("com.hivemq:hivemq-mqtt-client:1.3.0")
    implementation(platform("com.hivemq:hivemq-mqtt-client-websocket:1.3.0"))
    implementation(platform("com.hivemq:hivemq-mqtt-client-proxy:1.3.0"))
    implementation(platform("com.hivemq:hivemq-mqtt-client-epoll:1.3.0"))
    implementation("com.hivemq:hivemq-mqtt-client-reactor:1.3.0")

    implementation("com.github.EspressifApp:lib-esptouch-android:1.1.1")
    implementation("com.github.EspressifApp:lib-esptouch-v2-android:2.2.1")
}