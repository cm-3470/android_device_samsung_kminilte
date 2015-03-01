package com.cyanogenmod.settings.device;

import java.util.List;

import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.net.Uri;
import android.content.Context;
import android.content.Intent;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceClickListener;

public class PkfManager implements OnPreferenceClickListener {

    public static final String PACKAGE_PKFMANAGER = "com.christopher83.pkfmanager";

    private Context mContext;

    public PkfManager(Context context) {
        mContext = context;
    }

    public boolean isPackageExists(String targetPackage) {
        PackageManager pm = mContext.getPackageManager();
        List<ApplicationInfo> packages = pm.getInstalledApplications(0);
        for (ApplicationInfo packageInfo : packages) {
            if (packageInfo.packageName.equals(targetPackage))
                return true;
        }
        return false;
    }

    @SuppressLint("NewApi")
    public void launchPackage(String targetPackage) {
        PackageManager manager = mContext.getPackageManager();
        Intent intent = manager.getLaunchIntentForPackage(targetPackage);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        mContext.startActivity(intent);
    }

    @Override
    public boolean onPreferenceClick(Preference preference) {
        if (isPackageExists(PACKAGE_PKFMANAGER)) {
            launchPackage(PACKAGE_PKFMANAGER);
        } else {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse("market://details?id="+ PACKAGE_PKFMANAGER));
            mContext.startActivity(intent);
        }
        return true;
    }

}
