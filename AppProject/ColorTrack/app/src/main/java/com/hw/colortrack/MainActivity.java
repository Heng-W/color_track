package com.hw.colortrack;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.app.Dialog;
import android.os.*;
import android.util.*;
import android.view.*;
import android.view.View.OnClickListener;
import android.graphics.drawable.*;
import android.graphics.*;
import android.widget.*;
import android.widget.LinearLayout.OnClickListener;
import android.support.v7.view.menu.MenuPopupHelper;
import android.content.*;
import android.media.*;

import java.lang.reflect.Field;
import java.util.*;


public class MainActivity extends FragmentActivity {
    private final int FRAG_MAX = 3;
    private FragmentComponent[] fc = new FragmentComponent[FRAG_MAX];
    private Dialog loadDialog;
    private TextView titleTv;
    private Toast toast;
    private Client client;
    private int idx = FRAG_MAX;

    private PopupMenu popupMenu;

    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            // TODO: Implement this method
            switch (msg.what) {
                case 0:
                    refreshConnectView();
                    toast.setText("连接已关闭");
                    toast.show();
                    break;
                case 1:
                    refreshConnectView();
                    toast.setText("连接中断");
                    toast.show();
                    break;
                case 10:
                    clearDim();
                    DialogUtils.closeDialog(loadDialog);
                    if (client.isConnected) {
                        toast.setText("连接成功");
                        toast.show();
                        initMainView();
                    } else {
                        toast.setText("连接失败");
                        toast.show();

                        getWindow().getDecorView().setBackgroundColor(getResources().getColor(R.color.white));
                        getWindow().setStatusBarColor(getResources().getColor(R.color.primary));
                        getWindow().setNavigationBarColor(getResources().getColor(R.color.nav_bar));
                        setContentView(R.layout.activity_main);
                        titleTv = (TextView)findViewById(R.id.title_text);
                        LinearLayout menuIcon = (LinearLayout)findViewById(R.id.menu_icon);
                        initPopupMenu(menuIcon);
                        menuIcon.setOnClickListener(new OnClickListener() {
                            public void onClick(View v) {
                                showPopupMenu();
                            }

                        });
                        initFragmentComponent();

                        setSelect(0);
                    }
                    break;
                case 21:
                    toast.setText("摄像头复位成功");
                    toast.show();
                    break;
                case 22:
                    toast.setText("状态复位成功");
                    toast.show();
                    break;
                default:
                    break;
            }
            super.handleMessage(msg);
        }

    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.connect);

        StrictMode.setThreadPolicy(
            new StrictMode.ThreadPolicy.Builder() .detectDiskReads().detectDiskWrites().detectNetwork() .penaltyLog().build());
        StrictMode.setVmPolicy(new StrictMode.VmPolicy.Builder() .detectLeakedSqlLiteObjects().penaltyLog().penaltyDeath() .build());

        toast = Toast.makeText(this, "", Toast.LENGTH_SHORT);
        client = new Client();
        client.setMainHandler(mHandler);
        initConnectView();
    }

    public Client getClient() {
        return this.client;
    }


    public Handler getHandler() {
        return this.mHandler;
    }

    public int dp2px(float dpValue) {
        WindowManager wm = (WindowManager) this.getSystemService(Context.WINDOW_SERVICE);
        DisplayMetrics dm = new DisplayMetrics();
        wm.getDefaultDisplay().getMetrics(dm);
        float density = dm.density;
        return (int)(dpValue * density + 0.5f);
    }

    public int px2dp(float pxValue) {
        WindowManager wm = (WindowManager) this.getSystemService(Context.WINDOW_SERVICE);
        DisplayMetrics dm = new DisplayMetrics();
        wm.getDefaultDisplay().getMetrics(dm);
        float density = dm.density;
        return (int)(pxValue / density + 0.5f);
    }

    public int getScreenWidth() {
        return getWindowManager().getDefaultDisplay().getWidth();
    }

    public int getScreenHeight() {
        return getWindowManager().getDefaultDisplay().getHeight();
    }

    public void applyDim(float dimAmount) {
        ViewGroup parent = (ViewGroup) getWindow().getDecorView().getRootView();
        Drawable dim = new ColorDrawable(Color.BLACK);
        dim.setBounds(0, 0, parent.getWidth(), parent.getHeight());
        dim.setAlpha((int)(255 * dimAmount));
        ViewGroupOverlay overlay = parent.getOverlay();
        overlay.add(dim);
    }

    public void clearDim() {
        ViewGroup parent = (ViewGroup) getWindow().getDecorView().getRootView();
        ViewGroupOverlay overlay = parent.getOverlay();
        overlay.clear();
    }

    private void refreshConnectView() {
        client.isConnected = false;
        idx = FRAG_MAX;
        getWindow().getDecorView().setBackgroundColor(getResources().getColor(R.color.window_bg));
        getWindow().setStatusBarColor(getResources().getColor(R.color.window_bg));
        getWindow().setNavigationBarColor(getResources().getColor(R.color.window_bg));
        titleTv.setText(getResources().getString(R.string.app_name));
        setContentView(R.layout.connect);

        initConnectView();
        for (int i = 0; i < FRAG_MAX; i++) {
            fc[i].release();
        }
    }

    private void initMainView() {
        getWindow().getDecorView().setBackgroundColor(getResources().getColor(R.color.white));
        getWindow().setStatusBarColor(getResources().getColor(R.color.primary));
        getWindow().setNavigationBarColor(getResources().getColor(R.color.nav_bar));
        setContentView(R.layout.activity_main);
        titleTv = (TextView)findViewById(R.id.title_text);
        LinearLayout menuIcon = (LinearLayout)findViewById(R.id.menu_icon);
        initPopupMenu(menuIcon);
        menuIcon.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                showPopupMenu();
            }

        });
        initFragmentComponent();

        client.sendCmd(Command.init);
        setSelect(0);
    }

    private void initPopupMenu(View v) {
        popupMenu = new PopupMenu(this, v);
        getMenuInflater().inflate(R.menu.main, popupMenu.getMenu());

        popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getItemId()) {
                    case R.id.action_exit:
                        client.sendCmd(Command.exit);
                        break;
                    case R.id.action_reset:
                        client.sendCmd(Command.reset);
                        break;
                    case R.id.action_camera_reset:
                        client.sendCmd(Command.camera_reset);
                        break;

                    default:
                        break;
                }
                return true;
            }
        });
        popupMenu.setOnDismissListener(new PopupMenu.OnDismissListener() {
            @Override
            public void onDismiss(PopupMenu menu) {
                clearDim();
            }
        });
        try {
            Field field = popupMenu.getClass().getDeclaredField("mPopup");
            field.setAccessible(true);
            MenuPopupHelper helper = (MenuPopupHelper) field.get(popupMenu);
            helper.setForceShowIcon(true);
        } catch (Exception e) {
        }

    }

    private void showPopupMenu() {

        switch (idx) {
            case 0:
                //tstItem.setVisible(false);
                break;
            case 1:
                break;
            case 2:
                break;
            default:
                break;
        }
        popupMenu.show();
        applyDim(0.15f);
    }

    private void initConnectView() {
        Button connectBtn = (Button) findViewById(R.id.connect_button);
        connectBtn.setOnClickListener(new OnClickListener() {

            public void onClick(View v) {
                loadDialog = DialogUtils.createLoadingDialog(MainActivity.this, "连接中...");
                applyDim(0.2f);
                new Thread(new Runnable() {
                    public void run() {
                        client.connect();
                        Message msg = new Message();
                        msg.what = 10;
                        mHandler.sendMessage(msg);
                    }
                }).start();

            }

        });

    }

    private void initFragmentComponent() {
        fc[0] = new FragmentComponent(0, R.id.first_sel, R.id.first_tv);
        fc[1] = new FragmentComponent(1, R.id.second_sel, R.id.second_tv);
        fc[2] = new FragmentComponent(2, R.id.third_sel, R.id.third_tv);
    }

    private void setSelect(int i) {
        FragmentManager fm = getSupportFragmentManager();
        FragmentTransaction transaction = fm.beginTransaction();//创建一个事务
        if (idx != FRAG_MAX) {
            fc[idx].dismiss(transaction);
        }
        idx = i;
        fc[idx].display(transaction);
        transaction.commit();//提交事务
    }

    private class FragmentComponent {

        private Fragment fragTab;
        private LinearLayout fragSel;
        private TextView tv;
        private String label;
        private final int index;

        public FragmentComponent(int i, int fragSelId, int tvId) {
            index = i;
            fragSel = (LinearLayout)findViewById(fragSelId);
            tv = (TextView)findViewById(tvId);
            label = (String)tv.getText();

            fragSel.setOnClickListener(new OnClickListener() {
                public void onClick(View v) {
                    setSelect(index);
                }
            });
        }


        public void dismiss(FragmentTransaction transaction) {
            if (fragTab != null) {
                transaction.hide(fragTab);
                tv.setTextColor(getResources().getColor(R.color.dark_grey));
            }
        }

        public void display(FragmentTransaction transaction) {
            if (fragTab == null) {
                createFragment();
                transaction.add(R.id.frag_content, fragTab);//添加到Activity
            } else {
                transaction.show(fragTab);
            }
            tv.setTextColor(getResources().getColor(R.color.deep_blue));
            titleTv.setText(label);
        };

        public void release() {
            fragTab = null;
        }

        private void createFragment() {
            switch (index) {
                case 0:
                    fragTab = new FirstFragment();
                    break;
                case 1:
                    fragTab = new SecondFragment();
                    break;
                case 2:
                    fragTab = new ThirdFragment();
                    break;
                default:
                    break;
            }
        }

    }


}
