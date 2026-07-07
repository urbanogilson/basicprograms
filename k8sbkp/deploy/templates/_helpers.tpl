{{/*
Expand the name of the chart.
*/}}
{{- define "agents.name" -}}
{{- .Chart.Name | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Create a default fully qualified app name.
*/}}
{{- define "agents.fullname" -}}
{{- .Chart.Name | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Common labels
*/}}
{{- define "agents.labels" -}}
helm.sh/chart: {{ .Chart.Name }}-{{ .Chart.Version }}
app.kubernetes.io/managed-by: {{ .Release.Service }}
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}

{{/*
API selector labels
*/}}
{{- define "agents.api.selectorLabels" -}}
app.kubernetes.io/name: api
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}

{{/*
App selector labels
*/}}
{{- define "agents.app.selectorLabels" -}}
app.kubernetes.io/name: app
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}

{{/*
Full image reference (registry/repository:tag)
*/}}
{{- define "agents.api.image" -}}
{{ .Values.global.image.registry }}/{{ .Values.api.image.repository }}:{{ .Values.global.image.tag }}
{{- end }}

{{- define "agents.app.image" -}}
{{ .Values.global.image.registry }}/{{ .Values.app.image.repository }}:{{ .Values.global.image.tag }}
{{- end }}
