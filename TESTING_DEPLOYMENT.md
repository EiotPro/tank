# 🧪 TESTING & DEPLOYMENT GUIDE

## ✅ IMPLEMENTATION COMPLETE!

All files have been created successfully. Here's what's ready:

### 📁 Files Created (13 total):

#### CSS (5 files) ✅
- `tank/dashboard/css/variables.css`
- `tank/dashboard/css/base.css`
- `tank/dashboard/css/components.css`
- `tank/dashboard/css/dashboard.css`
- `tank/dashboard/css/animations.css`

#### JavaScript (7 files) ✅
- `tank/dashboard/js/config.js` ⚠️ **IMPORTANT: Contains LOCAL/PRODUCTION switch**
- `tank/dashboard/js/api.js`
- `tank/dashboard/js/ui.js`
- `tank/dashboard/js/chartManager.js`
- `tank/dashboard/js/theme.js`
- `tank/dashboard/js/notifications.js`
- `tank/dashboard/js/main.js`

#### PHP (1 file) ✅
- `tank/dashboard/index_new.php` (New dashboard)

---

## 🧪 TESTING LOCALLY

### Step 1: Verify File Structure
```powershell
# Check if all files exist
dir C:\xampp\htdocs\Tank\tank\dashboard\css
dir C:\xampp\htdocs\Tank\tank\dashboard\js
```

Expected output: You should see all CSS and JS files listed above.

### Step 2: Start XAMPP Services
1. Open XAMPP Control Panel
2. Start **Apache**
3. Start **MySQL**
4. Verify both are running (green indicators)

### Step 3: Access New Dashboard
Open your browser and navigate to:
```
http://localhost/tank/dashboard/index_new.php
```

### Step 4: Check Browser Console
1. Press **F12** to open Developer Tools
2. Go to **Console** tab
3. You should see:
   ```
   🚀 Tank Monitoring Dashboard
   📍 Environment: LOCAL
   🔗 API Base URL: http://localhost/tank/api
   ⏱️  Refresh Interval: 5s
   ```

### Step 5: Verify Features Work

#### ✅ Tank Visualization
- [ ] Tank should display with water level
- [ ] Percentage should be shown above tank
- [ ] Status badge should show tank status

#### ✅ Metric Cards
- [ ] Water Level (cm) should display
- [ ] Fill Percentage (%) should display
- [ ] Last Update time should show
- [ ] Progress bar should match percentage

#### ✅ Chart
- [ ] Chart should load with historical data
- [ ] Period buttons (1H, 6H, 24H) should work
- [ ] Hovering over chart should show tooltips

#### ✅ Interactive Features
- [ ] **Refresh Button** - Click and verify spinning animation
- [ ] **Theme Toggle** - Switch between light/dark mode
- [ ] **Toast Notifications** - Should appear on actions
- [ ] **Auto-refresh** - Wait 5 seconds, data should update

### Step 6: Test Error Handling
1. Stop MySQL in XAMPP
2. Refresh dashboard
3. Should see "Connection Error" toast notification
4. Restart MySQL
5. Click refresh - should reconnect

---

## 🚀 DEPLOYING TO PRODUCTION (iotlogic.in)

### Pre-Deployment Checklist

#### 1. Update Environment in config.js
```javascript
// File: tank/dashboard/js/config.js
// Line 5: Change this
const ENV = 'PRODUCTION'; // ⚠️ CHANGE FROM 'LOCAL'
```

#### 2. Verify Database Credentials
```php
// File: tank/api/config.php
// Check these values
define('DB_HOST', 'localhost');
define('DB_USER', 'dwljzndr_tank'); // Production user
define('DB_PASS', 'your_production_password'); // ⚠️ UPDATE
define('DB_NAME', 'dwljzndr_tank');
define('API_KEY', 'your_secure_key'); // ⚠️ UPDATE
```

#### 3. Update Hardware Config (if needed)
```cpp
// File: include/config.h
#define WEBSITE_HOST "iotlogic.in"
#define WEBSITE_PORT 443 // HTTPS
#define API_ENDPOINT "/tank/api/tank_update.php"
#define API_KEY "your_secure_key" // Must match PHP config
```

### Deployment Steps

#### Option A: Upload via FTP/SFTP
```
1. Connect to iotlogic.in via FTP
2. Navigate to /tank/dashboard/
3. Upload these folders:
   - css/ (all 5 files)
   - js/ (all 7 files)
4. Upload index_new.php
5. Rename index.php to index_old.php (backup)
6. Rename index_new.php to index.php
```

#### Option B: Upload via cPanel File Manager
```
1. Login to cPanel for iotlogic.in
2. Open File Manager
3. Navigate to public_html/tank/dashboard/
4. Upload css and js folders
5. Upload index_new.php
6. Backup old index.php
7. Rename index_new.php to index.php
```

#### Option C: Command Line (if SSH access)
```bash
# From your local machine
scp -r tank/dashboard/css user@iotlogic.in:/path/to/public_html/tank/dashboard/
scp -r tank/dashboard/js user@iotlogic.in:/path/to/public_html/tank/dashboard/
scp tank/dashboard/index_new.php user@iotlogic.in:/path/to/public_html/tank/dashboard/
```

### Post-Deployment Testing

#### 1. Access Production Dashboard
```
https://iotlogic.in/tank/dashboard/
```

#### 2. Check Console (F12)
Should show:
```
📍 Environment: PRODUCTION
🔗 API Base URL: https://iotlogic.in/tank/api
⏱️  Refresh Interval: 30s
```

#### 3. Verify All Features
- [ ] Dashboard loads without errors
- [ ] Data displays correctly
- [ ] Chart shows historical data
- [ ] Theme toggle works
- [ ] Auto-refresh works (30s interval)
- [ ] No console errors

#### 4. Test From Mobile Device
- [ ] Open on phone/tablet
- [ ] Check responsive design
- [ ] Verify all features work

---

## 🐛 TROUBLESHOOTING

### Issue: Dashboard doesn't load
**Solution:**
```
1. Check browser console for errors
2. Verify all CSS/JS files uploaded
3. Check file permissions (should be 644)
4. Clear browser cache (Ctrl+Shift+Del)
```

### Issue: "Failed to fetch data"
**Solution:**
```
1. Check API endpoint URL in config.js
2. Verify database is running
3. Check tank/api/config.php credentials
4. Test API directly: https://iotlogic.in/tank/api/get_tank_data.php
```

### Issue: Chart not displaying
**Solution:**
```
1. Check if Chart.js loaded (view source, check CDN)
2. Verify data exists in database
3. Check browser console for errors
4. Ensure canvas element ID matches: #tankChart
```

### Issue: Theme toggle not working
**Solution:**
```
1. Check if theme.js loaded
2. Verify button ID: #themeToggle
3. Clear localStorage: localStorage.clear()
4. Refresh page
```

### Issue: Auto-refresh not working
**Solution:**
```
1. Check config.js REFRESH_INTERVAL setting
2. Verify main.js loaded successfully
3. Check console for errors
4. Ensure API endpoints accessible
```

---

## 📊 PERFORMANCE CHECKLIST

### Local (Development)
- [ ] Page loads in < 2 seconds
- [ ] Chart renders smoothly
- [ ] No console errors
- [ ] Refresh works every 5 seconds
- [ ] Theme switching is instant

### Production
- [ ] Page loads in < 3 seconds
- [ ] HTTPS working (no mixed content warnings)
- [ ] Auto-refresh works (30s interval)
- [ ] Mobile responsive
- [ ] All features functional

---

## 🔄 ROLLBACK PLAN

If something goes wrong:

### Immediate Rollback
```
1. Rename index.php to index_broken.php
2. Rename index_old.php back to index.php
3. System reverts to old dashboard
```

### Debugging Mode
```javascript
// In config.js, temporarily enable:
const ENV = 'LOCAL'; // To see detailed logs
```

---

## 📝 MAINTENANCE

### Weekly Tasks
- [ ] Check dashboard loads properly
- [ ] Verify data is updating
- [ ] Review browser console for errors
- [ ] Test on different devices

### Monthly Tasks
- [ ] Review and clean old data (>30 days)
- [ ] Check database performance
- [ ] Update dependencies if needed
- [ ] Backup dashboard files

### Update Procedure
```
1. Test changes locally first
2. Update config.js environment to PRODUCTION
3. Upload modified files only
4. Test immediately after upload
5. Monitor for 24 hours
```

---

## 🎉 SUCCESS INDICATORS

Your deployment is successful if:

✅ Dashboard loads on iotlogic.in
✅ Real tank data displays
✅ Chart shows 24-hour history
✅ Auto-refresh works (every 30s in production)
✅ Theme toggle works
✅ Mobile responsive
✅ No console errors
✅ Hardware can POST data successfully

---

## 📞 SUPPORT

If you need help:
1. Check browser console (F12)
2. Check server error logs
3. Review this guide's troubleshooting section
4. Test locally first to isolate the issue

---

## 🎯 QUICK REFERENCE

### Files to Modify for Production:
1. **`js/config.js`** - Line 5: `const ENV = 'PRODUCTION'`
2. **`../api/config.php`** - Database credentials
3. **`../../include/config.h`** - Hardware config (if needed)

### Key URLs:
- **Local**: http://localhost/tank/dashboard/index_new.php
- **Production**: https://iotlogic.in/tank/dashboard/
- **API Test**: https://iotlogic.in/tank/api/get_tank_data.php

### Important IDs (for JS):
- `#tankWater` - Tank water element
- `#tankLevelText` - Percentage display
- `#tankChart` - Chart canvas
- `#themeToggle` - Theme button
- `#refreshBtn` - Refresh button

---

**Good luck with your deployment! 🚀**
