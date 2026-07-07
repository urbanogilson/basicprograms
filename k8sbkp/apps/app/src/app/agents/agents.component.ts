import { Component, OnInit } from '@angular/core';
import { AgentService, Agent } from './agent.service';

@Component({
  selector: 'app-agents',
  standalone: true,
  imports: [],
  templateUrl: './agents.component.html',
  styleUrl: './agents.component.css',
})
export class AgentsComponent implements OnInit {
  agents: Agent[] = [];
  error: string | null = null;

  constructor(private agentService: AgentService) {}

  ngOnInit(): void {
    this.agentService.getAgents().subscribe({
      next: (data) => (this.agents = data),
      error: (err) => (this.error = `Failed to load agents: ${err.message}`),
    });
  }
}
