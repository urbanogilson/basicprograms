{{/*
Chart name.
*/}}
{{- define "cerrado.name" -}}
{{- .Chart.Name | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Common labels.
*/}}
{{- define "cerrado.labels" -}}
helm.sh/chart: {{ .Chart.Name }}-{{ .Chart.Version }}
app.kubernetes.io/managed-by: {{ .Release.Service }}
app.kubernetes.io/instance: {{ .Release.Name }}
app.kubernetes.io/part-of: cerrado
{{- end }}

{{/*
API selector labels.
*/}}
{{- define "cerrado.api.selectorLabels" -}}
app.kubernetes.io/name: cerrado-api
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}

{{/*
Web selector labels.
*/}}
{{- define "cerrado.web.selectorLabels" -}}
app.kubernetes.io/name: cerrado-web
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}

{{/*
Full image references (registry/repository:tag).
*/}}
{{- define "cerrado.api.image" -}}
{{- with .Values.global.image.registry }}{{ . }}/{{ end }}{{ .Values.api.image.repository }}:{{ .Values.global.image.tag }}
{{- end }}

{{- define "cerrado.web.image" -}}
{{- with .Values.global.image.registry }}{{ . }}/{{ end }}{{ .Values.web.image.repository }}:{{ .Values.global.image.tag }}
{{- end }}

{{- define "cerrado.migrate.image" -}}
{{- with .Values.global.image.registry }}{{ . }}/{{ end }}{{ .Values.migrate.image.repository }}:{{ .Values.global.image.tag }}
{{- end }}

{{/*
Name of the Secret holding DATABASE_URL: the provided existingSecret, or the
chart-created "cerrado-db" fallback when the escape-hatch url is set.
*/}}
{{- define "cerrado.db.secretName" -}}
{{- if .Values.externalDatabase.existingSecret -}}
{{ .Values.externalDatabase.existingSecret }}
{{- else -}}
cerrado-db
{{- end -}}
{{- end }}
