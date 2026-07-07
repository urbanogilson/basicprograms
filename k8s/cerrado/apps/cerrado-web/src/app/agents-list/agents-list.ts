import { HttpClient } from '@angular/common/http';
import { Component, inject, signal } from '@angular/core';
import { Agent } from '../agent';

@Component({
  selector: 'app-agents-list',
  imports: [],
  templateUrl: './agents-list.html',
  styleUrl: './agents-list.css',
})
export class AgentsList {
  private http = inject(HttpClient);

  protected agents = signal<Agent[]>([]);
  protected loading = signal(true);
  protected error = signal(false);

  constructor() {
    this.http.get<Agent[]>('/api/agents').subscribe({
      next: (agents) => {
        this.agents.set(agents);
        this.loading.set(false);
      },
      error: () => {
        this.error.set(true);
        this.loading.set(false);
      },
    });
  }
}
