# HƯỚNG DẪN PHÁT TRIỂN WEB APPLICATION - ELECTRIC-NOSE

## 📋 MỤC LỤC
1. [Tại sao chọn Web Application?](#1-tại-sao-chọn-web-application)
2. [Technology Stack](#2-technology-stack)
3. [Kiến trúc Frontend](#3-kiến-trúc-frontend)
4. [Setup Development Environment](#4-setup-development-environment)
5. [Cấu trúc Project](#5-cấu-trúc-project)
6. [Các Component chính](#6-các-component-chính)
7. [State Management](#7-state-management)
8. [API Integration](#8-api-integration)
9. [Responsive Design](#9-responsive-design)
10. [Deployment](#10-deployment)

---

## 1. TẠI SAO CHỌN WEB APPLICATION?

### **✅ Ưu điểm của Web App:**
- **Dễ phát triển**: Không cần kinh nghiệm mobile development
- **Cross-platform**: Chạy trên mọi thiết bị (PC, tablet, mobile)
- **Không cần App Store**: Deploy trực tiếp lên web
- **Dễ maintain**: Chỉ cần update 1 lần
- **Chi phí thấp**: Không cần native development
- **Responsive**: Tự động adapt với màn hình

### **🎯 Phù hợp cho:**
- Bắt đầu dự án nhanh chóng
- Không có kinh nghiệm mobile
- Cần prototype nhanh
- Budget hạn chế

---

## 2. TECHNOLOGY STACK

### **🌐 Frontend Framework Options**

#### **A. React.js (Khuyến nghị)**
```
✅ Phổ biến nhất
✅ Học việc dễ dàng
✅ Community lớn
✅ Nhiều thư viện hỗ trợ
✅ Job market tốt

Tech Stack:
- React 18+
- TypeScript
- Vite (Build tool)
- Tailwind CSS
- React Query (Data fetching)
- React Router (Routing)
```

#### **B. Vue.js (Alternative)**
```
✅ Dễ học hơn React
✅ Template syntax quen thuộc
✅ Performance tốt
✅ Documentation tốt

Tech Stack:
- Vue 3+
- TypeScript
- Vite
- Tailwind CSS
- Pinia (State management)
- Vue Router
```

#### **C. Angular (Enterprise)**
```
✅ Enterprise-ready
✅ TypeScript native
✅ Full framework
✅ Testing tích hợp

Tech Stack:
- Angular 16+
- TypeScript
- Angular CLI
- Angular Material
- RxJS
```

### **🎨 UI Framework Options**

#### **A. Tailwind CSS (Khuyến nghị)**
```
✅ Utility-first CSS
✅ Customizable
✅ Responsive design
✅ Performance tốt
✅ Học nhanh
```

#### **B. Material-UI (MUI)**
```
✅ Google Material Design
✅ Component sẵn có
✅ Theme system
✅ Accessibility tốt
```

#### **C. Ant Design**
```
✅ Enterprise UI
✅ Component phong phú
✅ Documentation tốt
✅ TypeScript support
```

---

## 3. KIẾN TRÚC FRONTEND

### **🏗️ Kiến trúc tổng quan**

```
┌─────────────────────────────────────────────────────────────────┐
│                    WEB APPLICATION LAYERS                       │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────┐ │
│  │   UI Layer  │  │  State      │  │  Service    │  │  API    │ │
│  │ (Components)│  │ Management  │  │   Layer     │  │ Layer   │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────┘ │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────┐ │
│  │  Routing    │  │  Auth       │  │  Utils      │  │  Types  │ │
│  │  System     │  │  System     │  │  & Hooks    │  │  & API  │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### **📁 Cấu trúc thư mục**

```
src/
├── components/         # Reusable components
│   ├── common/         # Common UI components
│   ├── forms/          # Form components
│   ├── charts/         # Chart components
│   └── layout/         # Layout components
├── pages/              # Page components
│   ├── Dashboard/
│   ├── Devices/
│   ├── Data/
│   ├── Reports/
│   └── Settings/
├── hooks/              # Custom React hooks
├── services/           # API services
├── store/              # State management
├── utils/              # Utility functions
├── types/              # TypeScript types
├── constants/          # App constants
└── assets/             # Static assets
```

---

## 4. SETUP DEVELOPMENT ENVIRONMENT

### **🛠️ Prerequisites**

```bash
# Node.js (LTS version)
node --version  # v18.17.0+

# npm hoặc yarn
npm --version   # 9.6.7+
# hoặc
yarn --version  # 1.22.19+

# Git
git --version   # 2.30+
```

### **⚡ React.js Setup**

```bash
# Tạo project mới với Vite
npm create vite@latest electric-nose-web -- --template react-ts

# Di chuyển vào thư mục
cd electric-nose-web

# Cài đặt dependencies
npm install

# Cài đặt thêm packages cần thiết
npm install @tanstack/react-query axios react-router-dom
npm install @headlessui/react @heroicons/react
npm install tailwindcss @tailwindcss/forms
npm install recharts date-fns
npm install react-hook-form @hookform/resolvers zod

# Cài đặt dev dependencies
npm install -D @types/node
```

### **🎨 Tailwind CSS Setup**

```bash
# Khởi tạo Tailwind
npx tailwindcss init -p

# Cấu hình tailwind.config.js
```

```javascript
// tailwind.config.js
/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        primary: {
          50: '#eff6ff',
          500: '#3b82f6',
          600: '#2563eb',
          700: '#1d4ed8',
        }
      }
    },
  },
  plugins: [
    require('@tailwindcss/forms'),
  ],
}
```

---

## 5. CẤU TRÚC PROJECT

### **📦 Package.json**

```json
{
  "name": "electric-nose-web",
  "private": true,
  "version": "0.0.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "tsc && vite build",
    "lint": "eslint . --ext ts,tsx --report-unused-disable-directives --max-warnings 0",
    "preview": "vite preview"
  },
  "dependencies": {
    "react": "^18.2.0",
    "react-dom": "^18.2.0",
    "@tanstack/react-query": "^4.32.6",
    "axios": "^1.5.0",
    "react-router-dom": "^6.15.0",
    "recharts": "^2.8.0",
    "react-hook-form": "^7.45.4",
    "zod": "^3.22.4",
    "date-fns": "^2.30.0"
  },
  "devDependencies": {
    "@types/react": "^18.2.15",
    "@types/react-dom": "^18.2.7",
    "@typescript-eslint/eslint-plugin": "^6.0.0",
    "@typescript-eslint/parser": "^6.0.0",
    "@vitejs/plugin-react": "^4.0.3",
    "autoprefixer": "^10.4.15",
    "eslint": "^8.45.0",
    "eslint-plugin-react-hooks": "^4.6.0",
    "eslint-plugin-react-refresh": "^0.4.3",
    "postcss": "^8.4.29",
    "tailwindcss": "^3.3.3",
    "typescript": "^5.0.2",
    "vite": "^4.4.5"
  }
}
```

### **🔧 Vite Configuration**

```typescript
// vite.config.ts
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  server: {
    port: 3000,
    proxy: {
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      }
    }
  }
})
```

---

## 6. CÁC COMPONENT CHÍNH

### **🏠 Dashboard Component**

```tsx
// src/pages/Dashboard/Dashboard.tsx
import React from 'react';
import { useQuery } from '@tanstack/react-query';
import { DeviceCard } from '../../components/DeviceCard';
import { DataChart } from '../../components/DataChart';
import { RecentData } from '../../components/RecentData';

export const Dashboard: React.FC = () => {
  const { data: devices, isLoading } = useQuery({
    queryKey: ['devices'],
    queryFn: () => fetchDevices(),
  });

  const { data: sensorData } = useQuery({
    queryKey: ['sensor-data'],
    queryFn: () => fetchSensorData(),
    refetchInterval: 5000, // Refresh every 5 seconds
  });

  if (isLoading) {
    return <div className="flex justify-center items-center h-64">
      <div className="animate-spin rounded-full h-32 w-32 border-b-2 border-primary-500"></div>
    </div>;
  }

  return (
    <div className="min-h-screen bg-gray-50">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8">
        <h1 className="text-3xl font-bold text-gray-900 mb-8">
          Electric-Nose Dashboard
        </h1>
        
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          {/* Device Status */}
          <div className="lg:col-span-2">
            <h2 className="text-xl font-semibold mb-4">Thiết bị</h2>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
              {devices?.map(device => (
                <DeviceCard key={device.id} device={device} />
              ))}
            </div>
          </div>

          {/* Recent Data */}
          <div>
            <h2 className="text-xl font-semibold mb-4">Dữ liệu gần đây</h2>
            <RecentData data={sensorData} />
          </div>
        </div>

        {/* Charts */}
        <div className="mt-8">
          <h2 className="text-xl font-semibold mb-4">Biểu đồ dữ liệu</h2>
          <DataChart data={sensorData} />
        </div>
      </div>
    </div>
  );
};
```

### **📱 Device Card Component**

```tsx
// src/components/DeviceCard.tsx
import React from 'react';
import { Device } from '../../types/device';

interface DeviceCardProps {
  device: Device;
}

export const DeviceCard: React.FC<DeviceCardProps> = ({ device }) => {
  const getStatusColor = (status: string) => {
    switch (status) {
      case 'online': return 'bg-green-100 text-green-800';
      case 'offline': return 'bg-red-100 text-red-800';
      case 'measuring': return 'bg-blue-100 text-blue-800';
      default: return 'bg-gray-100 text-gray-800';
    }
  };

  return (
    <div className="bg-white rounded-lg shadow-md p-6">
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-semibold text-gray-900">
          {device.name}
        </h3>
        <span className={`px-2 py-1 rounded-full text-xs font-medium ${getStatusColor(device.status)}`}>
          {device.status}
        </span>
      </div>
      
      <div className="space-y-2">
        <div className="flex justify-between">
          <span className="text-sm text-gray-600">Nhiệt độ:</span>
          <span className="text-sm font-medium">{device.temperature}°C</span>
        </div>
        <div className="flex justify-between">
          <span className="text-sm text-gray-600">Độ ẩm:</span>
          <span className="text-sm font-medium">{device.humidity}%</span>
        </div>
        <div className="flex justify-between">
          <span className="text-sm text-gray-600">Cập nhật:</span>
          <span className="text-sm font-medium">{device.lastUpdate}</span>
        </div>
      </div>

      <div className="mt-4 flex space-x-2">
        <button className="flex-1 bg-primary-500 text-white px-4 py-2 rounded-md text-sm font-medium hover:bg-primary-600">
          Điều khiển
        </button>
        <button className="flex-1 bg-gray-200 text-gray-800 px-4 py-2 rounded-md text-sm font-medium hover:bg-gray-300">
          Chi tiết
        </button>
      </div>
    </div>
  );
};
```

### **📊 Data Chart Component**

```tsx
// src/components/DataChart.tsx
import React from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';

interface DataChartProps {
  data: any[];
}

export const DataChart: React.FC<DataChartProps> = ({ data }) => {
  return (
    <div className="bg-white rounded-lg shadow-md p-6">
      <h3 className="text-lg font-semibold mb-4">Biểu đồ nhiệt độ và độ ẩm</h3>
      <ResponsiveContainer width="100%" height={300}>
        <LineChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="time" />
          <YAxis />
          <Tooltip />
          <Line 
            type="monotone" 
            dataKey="temperature" 
            stroke="#3b82f6" 
            strokeWidth={2}
            name="Nhiệt độ (°C)"
          />
          <Line 
            type="monotone" 
            dataKey="humidity" 
            stroke="#10b981" 
            strokeWidth={2}
            name="Độ ẩm (%)"
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};
```

---

## 7. STATE MANAGEMENT

### **🔄 React Query Setup**

```tsx
// src/main.tsx
import React from 'react'
import ReactDOM from 'react-dom/client'
import { QueryClient, QueryClientProvider } from '@tanstack/react-query'
import { BrowserRouter } from 'react-router-dom'
import App from './App.tsx'
import './index.css'

const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      staleTime: 5 * 60 * 1000, // 5 minutes
      refetchOnWindowFocus: false,
    },
  },
})

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <QueryClientProvider client={queryClient}>
      <BrowserRouter>
        <App />
      </BrowserRouter>
    </QueryClientProvider>
  </React.StrictMode>,
)
```

### **🔐 Auth Context**

```tsx
// src/contexts/AuthContext.tsx
import React, { createContext, useContext, useState, useEffect } from 'react';

interface User {
  id: string;
  email: string;
  name: string;
}

interface AuthContextType {
  user: User | null;
  login: (email: string, password: string) => Promise<void>;
  logout: () => void;
  isLoading: boolean;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

export const AuthProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [user, setUser] = useState<User | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    // Check for existing token
    const token = localStorage.getItem('token');
    if (token) {
      // Validate token and get user info
      fetchUserInfo(token);
    } else {
      setIsLoading(false);
    }
  }, []);

  const login = async (email: string, password: string) => {
    const response = await fetch('/api/auth/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ email, password }),
    });

    if (response.ok) {
      const { token, user } = await response.json();
      localStorage.setItem('token', token);
      setUser(user);
    } else {
      throw new Error('Login failed');
    }
  };

  const logout = () => {
    localStorage.removeItem('token');
    setUser(null);
  };

  const fetchUserInfo = async (token: string) => {
    try {
      const response = await fetch('/api/auth/me', {
        headers: { Authorization: `Bearer ${token}` },
      });
      if (response.ok) {
        const user = await response.json();
        setUser(user);
      }
    } catch (error) {
      console.error('Failed to fetch user info:', error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <AuthContext.Provider value={{ user, login, logout, isLoading }}>
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => {
  const context = useContext(AuthContext);
  if (context === undefined) {
    throw new Error('useAuth must be used within an AuthProvider');
  }
  return context;
};
```

---

## 8. API INTEGRATION

### **🔌 API Service**

```tsx
// src/services/api.ts
import axios from 'axios';

const api = axios.create({
  baseURL: '/api',
  timeout: 10000,
});

// Request interceptor để thêm token
api.interceptors.request.use((config) => {
  const token = localStorage.getItem('token');
  if (token) {
    config.headers.Authorization = `Bearer ${token}`;
  }
  return config;
});

// Response interceptor để xử lý lỗi
api.interceptors.response.use(
  (response) => response,
  (error) => {
    if (error.response?.status === 401) {
      localStorage.removeItem('token');
      window.location.href = '/login';
    }
    return Promise.reject(error);
  }
);

export const deviceApi = {
  getDevices: () => api.get('/devices'),
  getDevice: (id: string) => api.get(`/devices/${id}`),
  connectDevice: (id: string) => api.post(`/devices/${id}/connect`),
  controlDevice: (id: string, command: any) => api.post(`/devices/${id}/control`, command),
};

export const dataApi = {
  getSensorData: (deviceId?: string, startDate?: string, endDate?: string) => 
    api.get('/data/sensor', { params: { deviceId, startDate, endDate } }),
  getLatestData: (deviceId: string) => api.get(`/data/sensor/latest/${deviceId}`),
};

export const reportApi = {
  generateReport: (deviceId: string, startDate: string, endDate: string, format: 'pdf' | 'excel') =>
    api.post('/reports/generate', { deviceId, startDate, endDate, format }),
  downloadReport: (reportId: string) => api.get(`/reports/${reportId}/download`),
};

export default api;
```

### **🎣 Custom Hooks**

```tsx
// src/hooks/useDevices.ts
import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { deviceApi } from '../services/api';

export const useDevices = () => {
  return useQuery({
    queryKey: ['devices'],
    queryFn: () => deviceApi.getDevices().then(res => res.data),
  });
};

export const useDevice = (id: string) => {
  return useQuery({
    queryKey: ['device', id],
    queryFn: () => deviceApi.getDevice(id).then(res => res.data),
    enabled: !!id,
  });
};

export const useConnectDevice = () => {
  const queryClient = useQueryClient();
  
  return useMutation({
    mutationFn: (id: string) => deviceApi.connectDevice(id),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ['devices'] });
    },
  });
};

export const useControlDevice = () => {
  const queryClient = useQueryClient();
  
  return useMutation({
    mutationFn: ({ id, command }: { id: string; command: any }) => 
      deviceApi.controlDevice(id, command),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ['devices'] });
    },
  });
};
```

---

## 9. RESPONSIVE DESIGN

### **📱 Mobile-First Approach**

```tsx
// src/components/ResponsiveLayout.tsx
import React from 'react';

interface ResponsiveLayoutProps {
  children: React.ReactNode;
}

export const ResponsiveLayout: React.FC<ResponsiveLayoutProps> = ({ children }) => {
  return (
    <div className="min-h-screen bg-gray-50">
      {/* Mobile Navigation */}
      <nav className="lg:hidden bg-white shadow-sm border-b">
        <div className="px-4 py-3">
          <div className="flex items-center justify-between">
            <h1 className="text-lg font-semibold">Electric-Nose</h1>
            <button className="p-2 rounded-md text-gray-600 hover:text-gray-900">
              <svg className="h-6 w-6" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 12h16M4 18h16" />
              </svg>
            </button>
          </div>
        </div>
      </nav>

      {/* Desktop Navigation */}
      <nav className="hidden lg:block bg-white shadow-sm border-b">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between h-16">
            <div className="flex items-center">
              <h1 className="text-xl font-semibold">Electric-Nose Dashboard</h1>
            </div>
            <div className="flex items-center space-x-4">
              <button className="text-gray-600 hover:text-gray-900">Settings</button>
              <button className="text-gray-600 hover:text-gray-900">Logout</button>
            </div>
          </div>
        </div>
      </nav>

      {/* Main Content */}
      <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6">
        {children}
      </main>
    </div>
  );
};
```

### **📊 Responsive Grid**

```tsx
// src/components/ResponsiveGrid.tsx
import React from 'react';

interface ResponsiveGridProps {
  children: React.ReactNode;
  cols?: {
    sm?: number;
    md?: number;
    lg?: number;
    xl?: number;
  };
}

export const ResponsiveGrid: React.FC<ResponsiveGridProps> = ({ 
  children, 
  cols = { sm: 1, md: 2, lg: 3, xl: 4 } 
}) => {
  const gridClasses = [
    'grid',
    'gap-4',
    `grid-cols-${cols.sm || 1}`,
    `md:grid-cols-${cols.md || 2}`,
    `lg:grid-cols-${cols.lg || 3}`,
    `xl:grid-cols-${cols.xl || 4}`,
  ].join(' ');

  return <div className={gridClasses}>{children}</div>;
};
```

---

## 10. DEPLOYMENT

### **🚀 Firebase Hosting (Khuyến nghị cho miễn phí)**

```bash
# Cài đặt Firebase CLI
npm install -g firebase-tools

# Login vào Firebase
firebase login

# Khởi tạo Firebase project
firebase init hosting

# Deploy
firebase deploy
```

**Ưu điểm Firebase:**
- ✅ **Miễn phí**: 10GB storage, 10GB transfer/tháng
- ✅ **CDN global**: Tốc độ tải nhanh
- ✅ **SSL tự động**: HTTPS miễn phí
- ✅ **Custom domain**: Hỗ trợ domain riêng
- ✅ **Rollback**: Dễ dàng rollback version
- ✅ **Analytics**: Firebase Analytics tích hợp

### **🌐 Vercel Deployment (Alternative)**

```bash
# Cài đặt Vercel CLI
npm i -g vercel

# Login vào Vercel
vercel login

# Deploy
vercel

# Deploy production
vercel --prod
```

### **🌐 Netlify Deployment (Alternative)**

```bash
# Cài đặt Netlify CLI
npm i -g netlify-cli

# Login
netlify login

# Build và deploy
npm run build
netlify deploy --prod --dir=dist
```

### **📦 Build Configuration**

```json
// package.json scripts
{
  "scripts": {
    "dev": "vite",
    "build": "tsc && vite build",
    "preview": "vite preview",
    "deploy": "npm run build && vercel --prod"
  }
}
```

### **🔧 Environment Variables**

```bash
# .env.local
VITE_API_BASE_URL=https://your-backend-url.com/api
VITE_WS_URL=wss://your-backend-url.com/ws
```

---

## 11. TESTING

### **🧪 Unit Testing với Vitest**

```bash
# Cài đặt testing dependencies
npm install -D vitest @testing-library/react @testing-library/jest-dom
```

```tsx
// src/components/__tests__/DeviceCard.test.tsx
import { render, screen } from '@testing-library/react';
import { DeviceCard } from '../DeviceCard';

const mockDevice = {
  id: '1',
  name: 'Test Device',
  status: 'online',
  temperature: 25.5,
  humidity: 60,
  lastUpdate: '2023-12-01 10:00:00'
};

describe('DeviceCard', () => {
  it('renders device information correctly', () => {
    render(<DeviceCard device={mockDevice} />);
    
    expect(screen.getByText('Test Device')).toBeInTheDocument();
    expect(screen.getByText('25.5°C')).toBeInTheDocument();
    expect(screen.getByText('60%')).toBeInTheDocument();
  });

  it('shows correct status color', () => {
    render(<DeviceCard device={mockDevice} />);
    
    const statusElement = screen.getByText('online');
    expect(statusElement).toHaveClass('bg-green-100', 'text-green-800');
  });
});
```

---

## 12. PERFORMANCE OPTIMIZATION

### **⚡ Code Splitting**

```tsx
// src/App.tsx
import { lazy, Suspense } from 'react';
import { Routes, Route } from 'react-router-dom';

const Dashboard = lazy(() => import('./pages/Dashboard'));
const Devices = lazy(() => import('./pages/Devices'));
const Data = lazy(() => import('./pages/Data'));

function App() {
  return (
    <Suspense fallback={<div>Loading...</div>}>
      <Routes>
        <Route path="/" element={<Dashboard />} />
        <Route path="/devices" element={<Devices />} />
        <Route path="/data" element={<Data />} />
      </Routes>
    </Suspense>
  );
}
```

### **🔄 Memoization**

```tsx
// src/components/OptimizedComponent.tsx
import React, { memo, useMemo } from 'react';

interface OptimizedComponentProps {
  data: any[];
  filter: string;
}

export const OptimizedComponent = memo<OptimizedComponentProps>(({ data, filter }) => {
  const filteredData = useMemo(() => {
    return data.filter(item => item.name.includes(filter));
  }, [data, filter]);

  return (
    <div>
      {filteredData.map(item => (
        <div key={item.id}>{item.name}</div>
      ))}
    </div>
  );
});
```

---

**🎯 Kết luận**: Web application là lựa chọn tuyệt vời để bắt đầu dự án Electric-Nose. Với React.js và Tailwind CSS, bạn có thể tạo ra một ứng dụng web hiện đại, responsive và dễ sử dụng mà không cần kinh nghiệm mobile development.
