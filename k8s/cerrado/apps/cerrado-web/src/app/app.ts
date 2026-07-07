import { Component } from '@angular/core';
import { RouterModule } from '@angular/router';
import { AgentsList } from './agents-list/agents-list';

@Component({
  imports: [AgentsList, RouterModule],
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.css',
})
export class App {
  protected title = 'cerrado-web';
}
