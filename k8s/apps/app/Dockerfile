# Build
FROM node:22-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY . .
RUN npx nx build app --configuration=production

# Runner
FROM nginx:alpine AS runner
COPY --from=builder /app/dist/apps/app/browser /usr/share/nginx/html
COPY apps/app/nginx.conf /etc/nginx/nginx.conf
RUN chown -R nginx:nginx /usr/share/nginx/html
USER nginx
EXPOSE 8080
CMD ["nginx", "-g", "daemon off;"]
